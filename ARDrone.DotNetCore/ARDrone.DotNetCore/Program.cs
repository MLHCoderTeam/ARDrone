﻿using System.IO;
using Microsoft.AspNetCore.Hosting;

namespace ARDrone.DotNetCore
{
    public class Program
    {
        public static void Main(string[] args)
        {
            var host = new WebHostBuilder()
                .UseKestrel()
                .UseContentRoot(Directory.GetCurrentDirectory())
                .UseIISIntegration()
                .UseStartup<Startup>()
                .UseApplicationInsights()
                .UseUrls("http://0.0.0.0")
                .Build();

            host.Run();
        }
    }
}
