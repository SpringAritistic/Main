using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Collections;
namespace TestConsole
{
    public partial class Program
    {
       public static void TestLink()
        {
            PriorityDocumentManager pdm = new PriorityDocumentManager();
            pdm.AddDocument(new Document("1", "Sample", 4));
            pdm.AddDocument(new Document("2", "Sample", 8));
            pdm.AddDocument(new Document("3", "Sample", 2));
            pdm.AddDocument(new Document("1", "Sample", 1));
            pdm.AddDocument(new Document("5", "Sample", 6));
            pdm.AddDocument(new Document("6", "Sample", 4));
            pdm.AddDocument(new Document("7", "Sample", 9));
            pdm.AddDocument(new Document("8", "Sample", 0));
            pdm.AddDocument(new Document("9", "Sample", 1));
            pdm.AddDocument(new Document("10", "Sample", 4));
            pdm.AddDocument(new Document("11", "Sample", 4));
            pdm.AddDocument(new Document("12", "Sample", 3));
            pdm.AddDocument(new Document("13", "Sample", 2));
            pdm.AddDocument(new Document("14", "Sample", 4));
            pdm.AddDocument(new Document("15", "Sample", 0));
            //pdm.AddDocument(new Document("16", "Sample", 5));
            //pdm.AddDocument(new Document("17", "Sample", 2));
            //pdm.AddDocument(new Document("18", "Sample", 9));
            //pdm.AddDocument(new Document("19", "Sample", 4));
            //pdm.AddDocument(new Document("20", "Sample", 1));
            //pdm.AddDocument(new Document("21", "Sample", 6));
            //pdm.AddDocument(new Document("22", "Sample", 2));
            //pdm.AddDocument(new Document("23", "Sample", 4));


           //
            pdm.DisplayAllList();
            pdm.DisplayAllNodes();
        }
    }
    public class Document
    {
        //private string title;
        public string Title { get; set; }
        public string Content {get;set;}
        public byte Priority { get; set; }
        public Document(string title,string content,byte priority)
        {
            Title = title;
            Content = content;
            Priority = priority;
        }
    }
    //
    public class PriorityDocumentManager
    {
        private readonly LinkedList<Document> documentList;
        private readonly List<LinkedListNode<Document>> priorityNodes;
        public PriorityDocumentManager()
        {
            documentList=new LinkedList<Document>();
            priorityNodes = new List<LinkedListNode<Document>>();
            for(int i=0;i<10;i++)
            {
                priorityNodes.Add(new LinkedListNode<Document>(null));
            }  
        }
        public void AddDocument(Document d)
        {
            if (d == null)
                throw new ArgumentException("d");
            AddDocumentToPriorityNode(d, d.Priority);
        }
        private void AddDocumentToPriorityNode(Document doc,int priority)
        {
            if (priority > 9 || priority < 0)
                throw new ArgumentException("priority must be between 0 and 9");
            if (priorityNodes[priority].Value==null)
            {
                if(--priority>=0)
                {
                    AddDocumentToPriorityNode(doc, priority);
                }
                else
                {
                    documentList.AddLast(doc);
                    priorityNodes[doc.Priority] = documentList.Last;
                }
                return;
            }
            else
            {
                LinkedListNode<Document> priorityNode=priorityNodes[priority];
                if(priority==doc.Priority)
                {
                    documentList.AddAfter(priorityNode, doc);
                    priorityNodes[doc.Priority] = priorityNode.Next;
                }
                else
                {
                    LinkedListNode<Document> firstPriorityNode = priorityNode;
                    while(firstPriorityNode.Previous!=null&& 
                        firstPriorityNode.Previous.Value.Priority==priorityNode.Value.Priority)
                    {
                        firstPriorityNode = priorityNode.Previous;
                    }
                    documentList.AddBefore(firstPriorityNode, doc);
                    priorityNodes[doc.Priority] = firstPriorityNode.Previous;
                }
            }
        }

        //
        public void DisplayAllNodes()
        {
            foreach (LinkedListNode<Document> node in priorityNodes)
            {

                Console.WriteLine();
                DisplayNode("上一个", node.Previous);
                DisplayNode("当前", node);
                DisplayNode("下一个", node.Next);
            }
            Console.WriteLine();
        }
        public void DisplayNode(string pre, LinkedListNode<Document> doc)
        {
            if (doc == null)
            {
                Console.WriteLine("{0}文档：\t不存在", pre);
                return;
            }
            if(doc.Value!=null)
                Console.WriteLine("{0}文档：\tpriority:{1},title:{2}", pre, doc.Value.Priority, doc.Value.Title);
            else
                Console.WriteLine("{0}文档：\t为空", pre);

        }

        public void DisplayAllList()
        {
            Console.WriteLine();
            Console.WriteLine("所有的文档有:");
            foreach (Document doc in documentList)
            {
                Console.WriteLine("priority:{0},title:{1}", doc.Priority, doc.Title);
            }
        }
        //
        public Document GetDocument()
        {
            Document doc = documentList.First.Value;
            documentList.RemoveFirst();
            return doc;
        }
    }

}
