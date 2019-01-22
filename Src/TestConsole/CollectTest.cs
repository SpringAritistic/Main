using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Collections;
namespace TestConsole
{
    public class person : IFormattable
    {
        public person() : this(string.Empty, 0) { }
        public person(person p) : this(p.name,p.age) { }

        public person(string _name, int _age) { name = _name; age = _age; }
        public string name;
        public int age;
        public person AddAge(int value)
        {
            age += value;
            return this;
        }
        public string ToString(string format, IFormatProvider formatProvider)
        {
            return ToString();
        }
        public override string ToString()
        {
            if (name.Length == 0 || age <= 0)
                return "";
            else
                return string.Format("姓名:{0,10},年龄:{1,3}", name, age);
        }
    }
    public partial class Program
    {
        static void testList()
        {

            List<person> persons = new List<person>(new person[] { new person("John", 20),
                new person("White", 21), new person("Smith", 19),new person("Kitty",22) });
            persons.ForEach(Console.WriteLine);//委托
            persons.Sort((p1, p2) =>
            {
               return  p1.age.CompareTo(p2.age);
            });
            Console.WriteLine("排序后：");
            persons.ForEach(p => { Console.WriteLine(p); });//莱姆达表达式
            Console.WriteLine("逆序后：");
            persons.Reverse();
            persons.ForEach(delegate(person p) { Console.WriteLine(p); });//匿名函数
            //删除
            Console.WriteLine();
            Console.WriteLine("删除年龄为21岁的，共删除了{0}个人", persons.RemoveAll(p => p.age == 21));
            persons.ForEach(Console.WriteLine);//委托
            Console.WriteLine();

            int index = persons.FindIndex(p => p.age < 21);
            Console.WriteLine("第一个年龄小于21岁的人是：{0}", persons[index]);
            index = persons.FindLastIndex(p => p.age < 21);
            Console.WriteLine("最后一个年龄小于21岁的人是：{0}", persons[index]);
            Console.WriteLine("查找到年龄为22岁的人是：{0}", persons.Find(p => p.age == 22));
            //所有人的年龄加一
            List<person> newPerson = persons.ConvertAll(p => new person(p.name, p.age + 1));
            Console.WriteLine("所有人的年龄加一");
            newPerson.ForEach(Console.WriteLine);
            //转换为只读list,加2岁
            Console.WriteLine("只读list");
            IReadOnlyList<person> readPerson = newPerson.AsReadOnly();
            foreach (person item in readPerson)
            {
                person one =new person(item);
                Console.WriteLine(one.AddAge(2));
            }
        }

    }
}
