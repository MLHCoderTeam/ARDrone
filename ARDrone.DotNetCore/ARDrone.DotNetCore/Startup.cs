using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using ARDrone.DotNetCore.Config;
using ARDrone.DotNetCore.Service;
using Microsoft.AspNetCore.Builder;
using Microsoft.AspNetCore.Hosting;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Logging;
using Microsoft.Extensions.Options;
using Twilio;

namespace ARDrone.DotNetCore
{
    public class Startup
    {
        public Startup(IHostingEnvironment env)
        {
            var builder = new ConfigurationBuilder()
                .SetBasePath(env.ContentRootPath)
                .AddJsonFile("appsettings.json", optional: false, reloadOnChange: true)
                .AddJsonFile($"appsettings.{env.EnvironmentName}.json", optional: true)
                .AddEnvironmentVariables();
            Configuration = builder.Build();
        }

        public IConfigurationRoot Configuration { get; }

        // This method gets called by the runtime. Use this method to add services to the container.
        public void ConfigureServices(IServiceCollection services)
        {
            services.AddOptions();

            services.Configure<TwilioConfig>(Configuration.GetSection(nameof(TwilioConfig)));
            services.Configure<DroneConfig>(Configuration.GetSection(nameof(DroneConfig)));

            services.AddSingleton<IHttpClientService, HttpClientService>();

            // Add framework services.
            services.AddMvcCore();
        }

        // This method gets called by the runtime. Use this method to configure the HTTP request pipeline.
        public void Configure(IApplicationBuilder app, IHostingEnvironment env, ILoggerFactory loggerFactory, IOptions<TwilioConfig> twilioConfig)
        {
            loggerFactory.AddConsole(Configuration.GetSection("Logging"));
            loggerFactory.AddDebug();

            TwilioClient.Init(twilioConfig.Value.AccountSid, twilioConfig.Value.AuthToken);

            app.UseMvc();
        }
    }
}
