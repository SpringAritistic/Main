using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

//
using System.Data.OleDb;

namespace CSharpApp
{
    public partial class Form1 : Form
    {
        OleDbConnection conn; 
        OleDbCommand da;
        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {

        }

        private void button1_Click(object sender, EventArgs e)
        {
            myLabel.BackColor = Color.Brown;
            myLabel.ForeColor = Color.Blue;
        }

        private void sex_Enter(object sender, EventArgs e)
        {
            //string str = "Hello";
        }

        private void FontList_SelectedIndexChanged(object sender, EventArgs e)
        {
            string str = "选择的字体风格：";
            ListBox.SelectedObjectCollection col = FontList.SelectedItems;
            if (col.Count > 0)
            {
                for (int i = 0; i < col.Count; i++)
                {
                    str += col[i];
                    if (i < col.Count - 1)
                        str += "、";
                }


            }

            labelFont.Text = str;
        }

        private void radioButtonMan_CheckedChanged(object sender, EventArgs e)
        {
            string str="你选择的性别为：";
            string strSex = "";
            RadioButton[] Buttons = { radioButtonMan, radioButton_Woman, radioButton_none };
            foreach (RadioButton mybtn in Buttons)
            {
                if (mybtn.Checked)
                {
                    strSex = mybtn.Text;
                    break;
                }
            }
            sexLabel.Text = str + strSex;

        }

        private void mylinkLabel_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            mylinkLabel.LinkVisited = true;
            System.Diagnostics.Process.Start("notepad");
        }

        private void mytimer_Tick(object sender, EventArgs e)
        {
            string str = System.DateTime.Now.ToString();
            labeltime.Text = str;
            status_time.Text = str;
        }

        private void 文件ToolStripMenuItem_Click(object sender, EventArgs e)
        {

        }

        private void 编辑ToolStripMenuItem_Click(object sender, EventArgs e)
        {

        }
                                
        private void 新建ToolStripMenuItem_Click(object sender, EventArgs e)
        {
                this.BackColor = this.BackColor == Color.Blue ? Color.Red : Color.Blue;
        }

        private void Form1_MouseMove(object sender, MouseEventArgs e)
        {
            toolPosition.Text = "X:" + e.X.ToString() + ",Y:" + e.Y.ToString();
        }

        private void subFrame_Click(object sender, EventArgs e)
        {
            Form2 subFrom = new Form2();
            subFrom.Show();//非模式
            //subFrom.Visible = false;
            //subFrom.ShowDialog(this);//模式
        }

        private void label1_Click(object sender, EventArgs e)
        {

        }
        string AddCode(string str)
        {
            
            return "'"+str+"'";
            
        }
        private void buttonAddData_Click(object sender, EventArgs e)
        {
            //有问题，留着以后改  2017/8/28
            if (textBoxNum.Text == "" || textBoxSex.Text == "" || textBoxAge.Text == "" || textBoxName.Text == "")
                MessageBox.Show("所有项都需要输入");
            string sourceStr ="Provider=Microsoft.Jet.OLEDB.4.0;Data Source=F:\\清风细雨\\练习\\数据库\\Person.mdb";
            string insertStr = "Insert Into StudentID(StudentNum,StudentName,StudentAge,StudentSex) Values(";
            insertStr += AddCode(textBoxNum.Text) + ",";
            insertStr +=  AddCode(textBoxName.Text) + ",";
            insertStr += textBoxAge.Text + ",";
            insertStr +=  AddCode(textBoxSex.Text) + ")";
            conn = new OleDbConnection(sourceStr);
            conn.Open();
            da = new OleDbCommand(insertStr,conn);
            //da.CommandText = insertStr;
            //da.Connection = conn;
            da.ExecuteNonQuery();
            textBoxNum.Text = "";
            textBoxName.Text = "";
            textBoxAge.Text = "";
            textBoxSex.Text = "";
            conn.Close();
        }
    }
}
