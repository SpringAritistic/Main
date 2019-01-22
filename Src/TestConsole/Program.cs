using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.Serialization;
using sharpDll;
using System.Text.RegularExpressions;
namespace TestConsole
{
   public partial class  Program
    {
        static void Main(string[] args)
        {
            Console.WriteLine("这是我的第一个C#项目");
            Console.WriteLine("Hello,world");
            //引用另一个dll
           mathTest myMath=new mathTest();
           Console.WriteLine(myMath.add(2, 3));
           Console.WriteLine(myMath.add(20, 3));
           Console.WriteLine(myMath.add(100, 1));
           findRegular();//测试正则表达式
           testList();
           TestLink();
           testExcept();
            Console.ReadKey();
        }
        //测试正则表达式
        public static void findRegular()
        {
            string str = @"Hello,world! My number is 1001, and my age is 27. This is my first extremely regular expression example!我 15 岁了";
            string pattern = @"\bex\S*";//ex开头，空格结尾的单词
            string patternNum = @"\b[0-9]+\b";//查找数字
            MatchCollection matchs = Regex.Matches(str, pattern, RegexOptions.IgnoreCase);
            showRegular(str, matchs);
            //
            matchs = Regex.Matches(str, patternNum, RegexOptions.IgnoreCase);
            showRegular(str, matchs);
            //
            str = @"http:\\www.wrox.com:4355";
            string patternURL = @"\b(\S+):\\(\S+):(\S+)\b";
            matchs = Regex.Matches(str, patternURL);
            showRegular(str, matchs);
            //替换
            Regex myReg = new Regex(patternURL);
            string strRep=@"$3 $2 $1";
            string resultStr = myReg.Replace(str, strRep);
            Console.WriteLine(resultStr);
            //
            string oldStr = "AFKLBCG1234";
            string findPattern = @"B+";
            string newStr = @"$_";
            myReg = new Regex(findPattern);
            Console.WriteLine(oldStr);
            Console.WriteLine(myReg.Replace(oldStr, newStr));
        }
        public static void showRegular(string oldstr,MatchCollection matchs)
        {
            Console.WriteLine();
            Console.WriteLine("原字符串为：{0}", oldstr);
            foreach (Match item in matchs)
            {
                Console.WriteLine("匹配项为第" + item.Index + "个字符:" + item.ToString());
            }
            Console.WriteLine();
        }

    }
}
