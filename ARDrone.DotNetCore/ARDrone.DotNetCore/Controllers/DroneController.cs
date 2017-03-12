using System;
using System.Net;
using System.Net.Http;
using System.Threading.Tasks;
using ARDrone.DotNetCore.Config;
using ARDrone.DotNetCore.Service;
using Microsoft.AspNetCore.Mvc;
using Microsoft.Extensions.Logging;
using Microsoft.Extensions.Options;

namespace ARDrone.DotNetCore.Controllers
{
    [Route("api/[controller]")]
    public class DroneController : Controller
    {
        private readonly IHttpClientService _httpClientService;
        private readonly DroneConfig _config;
        private readonly ILogger _logger;

        public DroneController(IOptions<DroneConfig> config, ILoggerFactory loggerFactory, IHttpClientService httpClientService)
        {
            _httpClientService = httpClientService;
            _config = config.Value;
            _logger = loggerFactory.CreateLogger<DroneController>();
        }

        [HttpPost]
        [Route("start")]
        public async Task<IActionResult> StartDrone()
        {
            try
            {
                var request = new HttpRequestMessage
                {
                    RequestUri = new Uri(_config.DroneServer),
                    Method = HttpMethod.Post
                };

                await _httpClientService.SendAsync(request).ConfigureAwait(false);

                return Ok();
            }
            catch (Exception ex)
            {
                _logger.LogError(ex.Message);
                return StatusCode((int)HttpStatusCode.InternalServerError);
            }
        }
    }
}
