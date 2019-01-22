using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;



namespace TestConsole
{
       public partial class  Program
       {
           static void testExcept()
           {
               Console.WriteLine();
               Console.WriteLine("测试异常处理");
               string fileName;
               Console.WriteLine("请输入一个文件名：");
               fileName = Console.ReadLine();
               myFileReader reader = new myFileReader();
               try
               {
                   reader.Open(fileName);
                   for (uint i = 0; i < reader.Count; i++)
                       reader.Next();
                   Console.WriteLine("所有信息已经读取完毕");
               }
               catch (FileNotFoundException ex)
               {
                   Console.WriteLine("{0}文件没有找到",fileName);
               }
               catch (fileFormatException ex)
               {
                   Console.WriteLine(ex.Message);
               }
               catch (fileEndException ex)
               {
                   Console.WriteLine(ex.Message);
               }
               catch (System.Exception ex)
               {
                   Console.WriteLine("产生了错误"+ex.Message);
               }
               finally
               {
                   reader.Dispose();
               }


           }

       }

    class myFileReader:IDisposable
    {
        private FileStream fs;
        private StreamReader sr;
        private uint count;
       public uint Count
       {
           get
           {
               if (isDisposed)
                   throw new ObjectDisposedException("文件已经打开了");

               if (!isOpen)
                   throw new fileNofoundException("文件没有打开");
               return count;
           }
           set
           {
               Count = value;
           }
       }  //个数
        private bool isOpen=false;
        private bool isDisposed=false;
        public void Open(string filename)
        {
            if (isDisposed)
                throw new ObjectDisposedException("文件已经打开了");
            fs = new FileStream(filename, FileMode.Open);
            sr = new StreamReader(fs);
            try
            {
                Count = uint.Parse(sr.ReadLine());
                isOpen = true;
            }
            catch (FormatException ex)
            {
                throw new fileFormatException("文件第一行不是个数字", ex);
            }
        }
        public  void Dispose()
        {
            if (isDisposed)
                return;
            isDisposed = true;
            isOpen = false;
            if (fs != null)
            {
                fs.Close();
                fs = null;
            }

        }
        public void Next()
        {
            if (isDisposed)
                throw new ObjectDisposedException("文件已经打开了");

            if (!isOpen)
                throw new fileNofoundException("文件没有打开");

            try
            {
                string name;
                name = sr.ReadLine();
                if (name == null)
                    throw new fileEndException("名字数目不够");
                if (name[0] == 'B')
                    throw new findSpecialException(name);
                Console.WriteLine(name);
            }
            catch (fileEndException ex)
            {
                Console.WriteLine(ex.Message);
            }
            finally
            {
            }
        }
    }
    class fileNofoundException:ApplicationException
    {
        public fileNofoundException(string message) : base(message + "文件未找到") { }
        public fileNofoundException(string message,Exception inner) : base(message + "文件未找到",inner) { }
    }
    class fileFormatException : ApplicationException
    {
        public fileFormatException(string message) : base(message) { }
        public fileFormatException(string message, Exception inner) : base(message , inner) { }
    }
    class fileEndException : ApplicationException
    {
        public fileEndException(string message) : base(message ) { }
        public fileEndException(string message, Exception inner) : base(message , inner) { }
    }
    class findSpecialException : ApplicationException
    {
        public findSpecialException(string name) : base("特别的人已找到，名字是"+name) { }
        public findSpecialException(string name, Exception inner) : base("特别的人已找到，名字是" + name) { }
    }

}
