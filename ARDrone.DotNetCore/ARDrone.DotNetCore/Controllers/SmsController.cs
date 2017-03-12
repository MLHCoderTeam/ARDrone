using System;
using System.Net;
using System.Threading.Tasks;
using ARDrone.DotNetCore.Config;
using Microsoft.AspNetCore.Mvc;
using Microsoft.Extensions.Logging;
using Microsoft.Extensions.Options;
using Twilio.Rest.Api.V2010.Account;
using Twilio.Types;

namespace ARDrone.DotNetCore.Controllers
{
    [Route("api/[controller]")]
    public class SmsController : Controller
    {
        private readonly TwilioConfig _config;
        private readonly ILogger _logger;

        public SmsController(IOptions<TwilioConfig> config, ILoggerFactory loggerFactory)
        {
            _config = config.Value;
            _logger = loggerFactory.CreateLogger<SmsController>();
        }

        [HttpPost]
        [Route("notify")]
        public async Task<IActionResult> Notify(string to, string message)
        {
            try
            {
                await MessageResource.CreateAsync(
                    from: new PhoneNumber(_config.SenderPhoneNumber),
                    to: new PhoneNumber(to),
                    body: message).ConfigureAwait(false);

                return NoContent();
            }
            catch(Exception ex)
            {
                _logger.LogError(ex.Message);
                return StatusCode((int)HttpStatusCode.InternalServerError);
            }
        }
    }
}
