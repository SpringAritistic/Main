namespace CSharpApp
{
    partial class Form1
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form1));
            this.myButton = new System.Windows.Forms.Button();
            this.contextMenuStrip1 = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.颜色ToolStripMenuItem1 = new System.Windows.Forms.ToolStripMenuItem();
            this.超链接ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.myLabel = new System.Windows.Forms.Label();
            this.sex = new System.Windows.Forms.GroupBox();
            this.radioButton_none = new System.Windows.Forms.RadioButton();
            this.radioButton_Woman = new System.Windows.Forms.RadioButton();
            this.radioButtonMan = new System.Windows.Forms.RadioButton();
            this.FontList = new System.Windows.Forms.ListBox();
            this.labelFont = new System.Windows.Forms.Label();
            this.sexLabel = new System.Windows.Forms.Label();
            this.toolTipOK = new System.Windows.Forms.ToolTip(this.components);
            this.mylinkLabel = new System.Windows.Forms.LinkLabel();
            this.mytimer = new System.Windows.Forms.Timer(this.components);
            this.labeltime = new System.Windows.Forms.Label();
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.文件ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.新建ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.保存ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.另存为ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.编辑ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.复制ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.粘贴ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.剪切ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.视图ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.顶视图ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.俯视图ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.设置ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.颜色ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.字体ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.帮助ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.版本ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.注册ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStrip1 = new System.Windows.Forms.ToolStrip();
            this.toolStripButton1 = new System.Windows.Forms.ToolStripButton();
            this.toolStripLabel1 = new System.Windows.Forms.ToolStripLabel();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.toolStripTextBox1 = new System.Windows.Forms.ToolStripTextBox();
            this.toolStripButton2 = new System.Windows.Forms.ToolStripButton();
            this.toolStripComboBox1 = new System.Windows.Forms.ToolStripComboBox();
            this.toolStripTextBox2 = new System.Windows.Forms.ToolStripTextBox();
            this.statusStrip1 = new System.Windows.Forms.StatusStrip();
            this.status_time = new System.Windows.Forms.ToolStripStatusLabel();
            this.toolPosition = new System.Windows.Forms.ToolStripStatusLabel();
            this.subFrame = new System.Windows.Forms.Button();
            this.textBoxNum = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.textBoxName = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.textBoxAge = new System.Windows.Forms.TextBox();
            this.buttonAddData = new System.Windows.Forms.Button();
            this.label4 = new System.Windows.Forms.Label();
            this.textBoxSex = new System.Windows.Forms.TextBox();
            this.contextMenuStrip1.SuspendLayout();
            this.sex.SuspendLayout();
            this.menuStrip1.SuspendLayout();
            this.toolStrip1.SuspendLayout();
            this.statusStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // myButton
            // 
            this.myButton.ContextMenuStrip = this.contextMenuStrip1;
            this.myButton.Location = new System.Drawing.Point(47, 52);
            this.myButton.Name = "myButton";
            this.myButton.Size = new System.Drawing.Size(75, 23);
            this.myButton.TabIndex = 0;
            this.myButton.Text = "确定";
            this.toolTipOK.SetToolTip(this.myButton, "这是一个确定按钮");
            this.myButton.UseVisualStyleBackColor = true;
            this.myButton.Click += new System.EventHandler(this.button1_Click);
            // 
            // contextMenuStrip1
            // 
            this.contextMenuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.颜色ToolStripMenuItem1,
            this.超链接ToolStripMenuItem});
            this.contextMenuStrip1.Name = "contextMenuStrip1";
            this.contextMenuStrip1.Size = new System.Drawing.Size(101, 48);
            // 
            // 颜色ToolStripMenuItem1
            // 
            this.颜色ToolStripMenuItem1.Name = "颜色ToolStripMenuItem1";
            this.颜色ToolStripMenuItem1.Size = new System.Drawing.Size(100, 22);
            this.颜色ToolStripMenuItem1.Text = "颜色";
            this.颜色ToolStripMenuItem1.Click += new System.EventHandler(this.新建ToolStripMenuItem_Click);
            // 
            // 超链接ToolStripMenuItem
            // 
            this.超链接ToolStripMenuItem.Name = "超链接ToolStripMenuItem";
            this.超链接ToolStripMenuItem.Size = new System.Drawing.Size(100, 22);
            this.超链接ToolStripMenuItem.Text = "文件";
            this.超链接ToolStripMenuItem.Click += new System.EventHandler(this.文件ToolStripMenuItem_Click);
            // 
            // myLabel
            // 
            this.myLabel.AutoSize = true;
            this.myLabel.ForeColor = System.Drawing.Color.Red;
            this.myLabel.Location = new System.Drawing.Point(32, 106);
            this.myLabel.Name = "myLabel";
            this.myLabel.Size = new System.Drawing.Size(71, 12);
            this.myLabel.TabIndex = 1;
            this.myLabel.Text = "Hello,World";
            // 
            // sex
            // 
            this.sex.Controls.Add(this.radioButton_none);
            this.sex.Controls.Add(this.radioButton_Woman);
            this.sex.Controls.Add(this.radioButtonMan);
            this.sex.Location = new System.Drawing.Point(69, 211);
            this.sex.Name = "sex";
            this.sex.Size = new System.Drawing.Size(200, 100);
            this.sex.TabIndex = 2;
            this.sex.TabStop = false;
            this.sex.Text = "性别";
            this.sex.Enter += new System.EventHandler(this.sex_Enter);
            // 
            // radioButton_none
            // 
            this.radioButton_none.AutoSize = true;
            this.radioButton_none.Location = new System.Drawing.Point(7, 78);
            this.radioButton_none.Name = "radioButton_none";
            this.radioButton_none.Size = new System.Drawing.Size(35, 16);
            this.radioButton_none.TabIndex = 2;
            this.radioButton_none.TabStop = true;
            this.radioButton_none.Text = "无";
            this.radioButton_none.UseVisualStyleBackColor = true;
            this.radioButton_none.CheckedChanged += new System.EventHandler(this.radioButtonMan_CheckedChanged);
            // 
            // radioButton_Woman
            // 
            this.radioButton_Woman.AutoSize = true;
            this.radioButton_Woman.Location = new System.Drawing.Point(7, 53);
            this.radioButton_Woman.Name = "radioButton_Woman";
            this.radioButton_Woman.Size = new System.Drawing.Size(35, 16);
            this.radioButton_Woman.TabIndex = 1;
            this.radioButton_Woman.TabStop = true;
            this.radioButton_Woman.Text = "女";
            this.radioButton_Woman.UseVisualStyleBackColor = true;
            this.radioButton_Woman.CheckedChanged += new System.EventHandler(this.radioButtonMan_CheckedChanged);
            // 
            // radioButtonMan
            // 
            this.radioButtonMan.AutoSize = true;
            this.radioButtonMan.Location = new System.Drawing.Point(7, 31);
            this.radioButtonMan.Name = "radioButtonMan";
            this.radioButtonMan.Size = new System.Drawing.Size(35, 16);
            this.radioButtonMan.TabIndex = 0;
            this.radioButtonMan.TabStop = true;
            this.radioButtonMan.Text = "男";
            this.radioButtonMan.UseVisualStyleBackColor = true;
            this.radioButtonMan.CheckedChanged += new System.EventHandler(this.radioButtonMan_CheckedChanged);
            // 
            // FontList
            // 
            this.FontList.FormattingEnabled = true;
            this.FontList.ItemHeight = 12;
            this.FontList.Items.AddRange(new object[] {
            "宋体",
            "斜体",
            "楷体",
            "仿宋"});
            this.FontList.Location = new System.Drawing.Point(327, 90);
            this.FontList.Name = "FontList";
            this.FontList.SelectionMode = System.Windows.Forms.SelectionMode.MultiExtended;
            this.FontList.Size = new System.Drawing.Size(120, 88);
            this.FontList.TabIndex = 3;
            this.FontList.SelectedIndexChanged += new System.EventHandler(this.FontList_SelectedIndexChanged);
            // 
            // labelFont
            // 
            this.labelFont.AutoSize = true;
            this.labelFont.Location = new System.Drawing.Point(323, 62);
            this.labelFont.Name = "labelFont";
            this.labelFont.Size = new System.Drawing.Size(0, 12);
            this.labelFont.TabIndex = 4;
            // 
            // sexLabel
            // 
            this.sexLabel.AutoSize = true;
            this.sexLabel.Location = new System.Drawing.Point(69, 318);
            this.sexLabel.Name = "sexLabel";
            this.sexLabel.Size = new System.Drawing.Size(0, 12);
            this.sexLabel.TabIndex = 5;
            // 
            // mylinkLabel
            // 
            this.mylinkLabel.AutoSize = true;
            this.mylinkLabel.Location = new System.Drawing.Point(76, 386);
            this.mylinkLabel.Name = "mylinkLabel";
            this.mylinkLabel.Size = new System.Drawing.Size(65, 12);
            this.mylinkLabel.TabIndex = 6;
            this.mylinkLabel.TabStop = true;
            this.mylinkLabel.Text = "超链接测试";
            this.mylinkLabel.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.mylinkLabel_LinkClicked);
            // 
            // mytimer
            // 
            this.mytimer.Enabled = true;
            this.mytimer.Interval = 1000;
            this.mytimer.Tick += new System.EventHandler(this.mytimer_Tick);
            // 
            // labeltime
            // 
            this.labeltime.AutoSize = true;
            this.labeltime.Location = new System.Drawing.Point(297, 368);
            this.labeltime.Name = "labeltime";
            this.labeltime.Size = new System.Drawing.Size(113, 12);
            this.labeltime.TabIndex = 7;
            this.labeltime.Text = "2017/8/21 16:56:27";
            // 
            // menuStrip1
            // 
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.文件ToolStripMenuItem,
            this.编辑ToolStripMenuItem,
            this.视图ToolStripMenuItem,
            this.设置ToolStripMenuItem,
            this.帮助ToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(699, 25);
            this.menuStrip1.TabIndex = 8;
            this.menuStrip1.Text = "menuStrip1";
            // 
            // 文件ToolStripMenuItem
            // 
            this.文件ToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.新建ToolStripMenuItem,
            this.保存ToolStripMenuItem,
            this.另存为ToolStripMenuItem});
            this.文件ToolStripMenuItem.Name = "文件ToolStripMenuItem";
            this.文件ToolStripMenuItem.Size = new System.Drawing.Size(66, 21);
            this.文件ToolStripMenuItem.Text = "文件（&F)";
            this.文件ToolStripMenuItem.Click += new System.EventHandler(this.文件ToolStripMenuItem_Click);
            // 
            // 新建ToolStripMenuItem
            // 
            this.新建ToolStripMenuItem.Checked = true;
            this.新建ToolStripMenuItem.CheckState = System.Windows.Forms.CheckState.Checked;
            this.新建ToolStripMenuItem.Name = "新建ToolStripMenuItem";
            this.新建ToolStripMenuItem.ShortcutKeyDisplayString = "";
            this.新建ToolStripMenuItem.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.N)));
            this.新建ToolStripMenuItem.Size = new System.Drawing.Size(147, 22);
            this.新建ToolStripMenuItem.Text = "新建";
            this.新建ToolStripMenuItem.Click += new System.EventHandler(this.新建ToolStripMenuItem_Click);
            // 
            // 保存ToolStripMenuItem
            // 
            this.保存ToolStripMenuItem.Name = "保存ToolStripMenuItem";
            this.保存ToolStripMenuItem.Size = new System.Drawing.Size(147, 22);
            this.保存ToolStripMenuItem.Text = "保存";
            // 
            // 另存为ToolStripMenuItem
            // 
            this.另存为ToolStripMenuItem.Name = "另存为ToolStripMenuItem";
            this.另存为ToolStripMenuItem.Size = new System.Drawing.Size(147, 22);
            this.另存为ToolStripMenuItem.Text = "另存为";
            // 
            // 编辑ToolStripMenuItem
            // 
            this.编辑ToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.复制ToolStripMenuItem,
            this.粘贴ToolStripMenuItem,
            this.剪切ToolStripMenuItem});
            this.编辑ToolStripMenuItem.Name = "编辑ToolStripMenuItem";
            this.编辑ToolStripMenuItem.Size = new System.Drawing.Size(59, 21);
            this.编辑ToolStripMenuItem.Text = "编辑(&E)";
            this.编辑ToolStripMenuItem.Click += new System.EventHandler(this.编辑ToolStripMenuItem_Click);
            // 
            // 复制ToolStripMenuItem
            // 
            this.复制ToolStripMenuItem.Name = "复制ToolStripMenuItem";
            this.复制ToolStripMenuItem.Size = new System.Drawing.Size(100, 22);
            this.复制ToolStripMenuItem.Text = "复制";
            // 
            // 粘贴ToolStripMenuItem
            // 
            this.粘贴ToolStripMenuItem.Name = "粘贴ToolStripMenuItem";
            this.粘贴ToolStripMenuItem.Size = new System.Drawing.Size(100, 22);
            this.粘贴ToolStripMenuItem.Text = "粘贴";
            // 
            // 剪切ToolStripMenuItem
            // 
            this.剪切ToolStripMenuItem.Name = "剪切ToolStripMenuItem";
            this.剪切ToolStripMenuItem.Size = new System.Drawing.Size(100, 22);
            this.剪切ToolStripMenuItem.Text = "剪切";
            // 
            // 视图ToolStripMenuItem
            // 
            this.视图ToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.顶视图ToolStripMenuItem,
            this.俯视图ToolStripMenuItem});
            this.视图ToolStripMenuItem.Name = "视图ToolStripMenuItem";
            this.视图ToolStripMenuItem.Size = new System.Drawing.Size(60, 21);
            this.视图ToolStripMenuItem.Text = "视图(&V)";
            // 
            // 顶视图ToolStripMenuItem
            // 
            this.顶视图ToolStripMenuItem.Name = "顶视图ToolStripMenuItem";
            this.顶视图ToolStripMenuItem.Size = new System.Drawing.Size(112, 22);
            this.顶视图ToolStripMenuItem.Text = "顶视图";
            // 
            // 俯视图ToolStripMenuItem
            // 
            this.俯视图ToolStripMenuItem.Name = "俯视图ToolStripMenuItem";
            this.俯视图ToolStripMenuItem.Size = new System.Drawing.Size(112, 22);
            this.俯视图ToolStripMenuItem.Text = "俯视图";
            // 
            // 设置ToolStripMenuItem
            // 
            this.设置ToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.颜色ToolStripMenuItem,
            this.字体ToolStripMenuItem});
            this.设置ToolStripMenuItem.Name = "设置ToolStripMenuItem";
            this.设置ToolStripMenuItem.Size = new System.Drawing.Size(59, 21);
            this.设置ToolStripMenuItem.Text = "设置(&S)";
            // 
            // 颜色ToolStripMenuItem
            // 
            this.颜色ToolStripMenuItem.Name = "颜色ToolStripMenuItem";
            this.颜色ToolStripMenuItem.Size = new System.Drawing.Size(100, 22);
            this.颜色ToolStripMenuItem.Text = "颜色";
            // 
            // 字体ToolStripMenuItem
            // 
            this.字体ToolStripMenuItem.Name = "字体ToolStripMenuItem";
            this.字体ToolStripMenuItem.Size = new System.Drawing.Size(100, 22);
            this.字体ToolStripMenuItem.Text = "字体";
            // 
            // 帮助ToolStripMenuItem
            // 
            this.帮助ToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.版本ToolStripMenuItem,
            this.注册ToolStripMenuItem});
            this.帮助ToolStripMenuItem.Name = "帮助ToolStripMenuItem";
            this.帮助ToolStripMenuItem.Size = new System.Drawing.Size(61, 21);
            this.帮助ToolStripMenuItem.Text = "帮助(&H)";
            // 
            // 版本ToolStripMenuItem
            // 
            this.版本ToolStripMenuItem.Name = "版本ToolStripMenuItem";
            this.版本ToolStripMenuItem.Size = new System.Drawing.Size(100, 22);
            this.版本ToolStripMenuItem.Text = "版本";
            // 
            // 注册ToolStripMenuItem
            // 
            this.注册ToolStripMenuItem.Name = "注册ToolStripMenuItem";
            this.注册ToolStripMenuItem.Size = new System.Drawing.Size(100, 22);
            this.注册ToolStripMenuItem.Text = "注册";
            // 
            // toolStrip1
            // 
            this.toolStrip1.ImeMode = System.Windows.Forms.ImeMode.On;
            this.toolStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripButton1,
            this.toolStripLabel1,
            this.toolStripSeparator1,
            this.toolStripTextBox1,
            this.toolStripButton2,
            this.toolStripComboBox1,
            this.toolStripTextBox2});
            this.toolStrip1.Location = new System.Drawing.Point(0, 25);
            this.toolStrip1.Name = "toolStrip1";
            this.toolStrip1.Size = new System.Drawing.Size(699, 25);
            this.toolStrip1.TabIndex = 9;
            this.toolStrip1.Text = "toolStrip1";
            // 
            // toolStripButton1
            // 
            this.toolStripButton1.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolStripButton1.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButton1.Image")));
            this.toolStripButton1.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButton1.Name = "toolStripButton1";
            this.toolStripButton1.Size = new System.Drawing.Size(23, 22);
            this.toolStripButton1.Text = "toolStripButton1";
            // 
            // toolStripLabel1
            // 
            this.toolStripLabel1.Name = "toolStripLabel1";
            this.toolStripLabel1.Size = new System.Drawing.Size(96, 22);
            this.toolStripLabel1.Text = "toolStripLabel1";
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(6, 25);
            // 
            // toolStripTextBox1
            // 
            this.toolStripTextBox1.Name = "toolStripTextBox1";
            this.toolStripTextBox1.Size = new System.Drawing.Size(100, 25);
            // 
            // toolStripButton2
            // 
            this.toolStripButton2.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolStripButton2.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButton2.Image")));
            this.toolStripButton2.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButton2.Name = "toolStripButton2";
            this.toolStripButton2.Size = new System.Drawing.Size(23, 22);
            this.toolStripButton2.Text = "toolStripButton2";
            // 
            // toolStripComboBox1
            // 
            this.toolStripComboBox1.Name = "toolStripComboBox1";
            this.toolStripComboBox1.Size = new System.Drawing.Size(121, 25);
            // 
            // toolStripTextBox2
            // 
            this.toolStripTextBox2.Name = "toolStripTextBox2";
            this.toolStripTextBox2.Size = new System.Drawing.Size(100, 25);
            // 
            // statusStrip1
            // 
            this.statusStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.status_time,
            this.toolPosition});
            this.statusStrip1.Location = new System.Drawing.Point(0, 651);
            this.statusStrip1.Name = "statusStrip1";
            this.statusStrip1.Size = new System.Drawing.Size(699, 22);
            this.statusStrip1.TabIndex = 10;
            this.statusStrip1.Text = "statusStrip1";
            // 
            // status_time
            // 
            this.status_time.Name = "status_time";
            this.status_time.Size = new System.Drawing.Size(0, 17);
            // 
            // toolPosition
            // 
            this.toolPosition.Name = "toolPosition";
            this.toolPosition.Size = new System.Drawing.Size(0, 17);
            // 
            // subFrame
            // 
            this.subFrame.Location = new System.Drawing.Point(362, 264);
            this.subFrame.Name = "subFrame";
            this.subFrame.Size = new System.Drawing.Size(75, 23);
            this.subFrame.TabIndex = 11;
            this.subFrame.Text = "子窗体";
            this.subFrame.UseVisualStyleBackColor = true;
            this.subFrame.Click += new System.EventHandler(this.subFrame_Click);
            // 
            // textBoxNum
            // 
            this.textBoxNum.Location = new System.Drawing.Point(78, 435);
            this.textBoxNum.Name = "textBoxNum";
            this.textBoxNum.Size = new System.Drawing.Size(100, 21);
            this.textBoxNum.TabIndex = 12;
            this.textBoxNum.Text = "20123";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(36, 440);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(29, 12);
            this.label1.TabIndex = 13;
            this.label1.Text = "学号";
            this.label1.Click += new System.EventHandler(this.label1_Click);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(36, 467);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(29, 12);
            this.label2.TabIndex = 15;
            this.label2.Text = "姓名";
            // 
            // textBoxName
            // 
            this.textBoxName.Location = new System.Drawing.Point(78, 462);
            this.textBoxName.Name = "textBoxName";
            this.textBoxName.Size = new System.Drawing.Size(100, 21);
            this.textBoxName.TabIndex = 14;
            this.textBoxName.Text = "Quick";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(36, 494);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(29, 12);
            this.label3.TabIndex = 17;
            this.label3.Text = "年龄";
            // 
            // textBoxAge
            // 
            this.textBoxAge.Location = new System.Drawing.Point(78, 489);
            this.textBoxAge.Name = "textBoxAge";
            this.textBoxAge.Size = new System.Drawing.Size(100, 21);
            this.textBoxAge.TabIndex = 16;
            this.textBoxAge.Text = "23";
            // 
            // buttonAddData
            // 
            this.buttonAddData.Location = new System.Drawing.Point(38, 539);
            this.buttonAddData.Name = "buttonAddData";
            this.buttonAddData.Size = new System.Drawing.Size(75, 23);
            this.buttonAddData.TabIndex = 18;
            this.buttonAddData.Text = "增加记录";
            this.buttonAddData.UseVisualStyleBackColor = true;
            this.buttonAddData.Click += new System.EventHandler(this.buttonAddData_Click);
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(36, 522);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(29, 12);
            this.label4.TabIndex = 20;
            this.label4.Text = "性别";
            // 
            // textBoxSex
            // 
            this.textBoxSex.Location = new System.Drawing.Point(78, 517);
            this.textBoxSex.Name = "textBoxSex";
            this.textBoxSex.Size = new System.Drawing.Size(100, 21);
            this.textBoxSex.TabIndex = 19;
            this.textBoxSex.Text = "男";
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(699, 673);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.textBoxSex);
            this.Controls.Add(this.buttonAddData);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.textBoxAge);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.textBoxName);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.textBoxNum);
            this.Controls.Add(this.subFrame);
            this.Controls.Add(this.statusStrip1);
            this.Controls.Add(this.toolStrip1);
            this.Controls.Add(this.labeltime);
            this.Controls.Add(this.mylinkLabel);
            this.Controls.Add(this.sexLabel);
            this.Controls.Add(this.labelFont);
            this.Controls.Add(this.FontList);
            this.Controls.Add(this.sex);
            this.Controls.Add(this.myLabel);
            this.Controls.Add(this.myButton);
            this.Controls.Add(this.menuStrip1);
            this.MainMenuStrip = this.menuStrip1;
            this.Name = "Form1";
            this.Text = "C#的测试应用程序";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.MouseMove += new System.Windows.Forms.MouseEventHandler(this.Form1_MouseMove);
            this.contextMenuStrip1.ResumeLayout(false);
            this.sex.ResumeLayout(false);
            this.sex.PerformLayout();
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            this.toolStrip1.ResumeLayout(false);
            this.toolStrip1.PerformLayout();
            this.statusStrip1.ResumeLayout(false);
            this.statusStrip1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button myButton;
        private System.Windows.Forms.Label myLabel;
        private System.Windows.Forms.GroupBox sex;
        private System.Windows.Forms.RadioButton radioButton_none;
        private System.Windows.Forms.RadioButton radioButton_Woman;
        private System.Windows.Forms.RadioButton radioButtonMan;
        private System.Windows.Forms.ListBox FontList;
        private System.Windows.Forms.Label labelFont;
        private System.Windows.Forms.Label sexLabel;
        private System.Windows.Forms.ToolTip toolTipOK;
        private System.Windows.Forms.LinkLabel mylinkLabel;
        private System.Windows.Forms.Timer mytimer;
        private System.Windows.Forms.Label labeltime;
        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStripMenuItem 文件ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem 编辑ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem 视图ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem 设置ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem 帮助ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem 新建ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem 保存ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem 另存为ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem 复制ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem 粘贴ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem 剪切ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem 顶视图ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem 俯视图ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem 颜色ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem 字体ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem 版本ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem 注册ToolStripMenuItem;
        private System.Windows.Forms.ToolStrip toolStrip1;
        private System.Windows.Forms.ToolStripButton toolStripButton1;
        private System.Windows.Forms.ToolStripLabel toolStripLabel1;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripTextBox toolStripTextBox1;
        private System.Windows.Forms.ToolStripButton toolStripButton2;
        private System.Windows.Forms.ToolStripComboBox toolStripComboBox1;
        private System.Windows.Forms.ToolStripTextBox toolStripTextBox2;
        private System.Windows.Forms.StatusStrip statusStrip1;
        private System.Windows.Forms.ToolStripStatusLabel status_time;
        private System.Windows.Forms.ToolStripStatusLabel toolPosition;
        private System.Windows.Forms.ContextMenuStrip contextMenuStrip1;
        private System.Windows.Forms.ToolStripMenuItem 颜色ToolStripMenuItem1;
        private System.Windows.Forms.ToolStripMenuItem 超链接ToolStripMenuItem;
        private System.Windows.Forms.Button subFrame;
        private System.Windows.Forms.TextBox textBoxNum;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox textBoxName;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TextBox textBoxAge;
        private System.Windows.Forms.Button buttonAddData;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.TextBox textBoxSex;
    }
}

