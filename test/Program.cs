using System;
using System.Runtime.InteropServices;
using System.IO;
using System.Net.Http.Headers;
using System.Net;

namespace ConsoleApplication1
{
    class Program
    {
        public static void Main(String[] args)
        {
            string data = File.ReadAllText("./wave1422307.json");
            WebClient client = new WebClient();
            // Optionally specify an encoding for uploading and downloading strings.
            client.Encoding = System.Text.Encoding.UTF8;
            // Upload the data.
            string reply = client.UploadString("http://0.0.0.0:8080/input_data", data);
            // Display the server's response.
            Console.WriteLine(reply);
        }
    }
}