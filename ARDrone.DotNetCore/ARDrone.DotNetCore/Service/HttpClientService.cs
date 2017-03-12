using System;
using System.Net;
using System.Net.Http;
using System.Threading.Tasks;
using Newtonsoft.Json;

namespace ARDrone.DotNetCore.Service
{
    public interface IHttpClientService
    {
        Task SendAsync(HttpRequestMessage request);
        Task<T> SendAsync<T>(HttpRequestMessage request) where T : class;
    }

    public class HttpClientService : IHttpClientService
    {
        private readonly HttpClient _httpClient;

        public HttpClientService()
        {
            _httpClient = new HttpClient
            {
                Timeout = TimeSpan.FromMinutes(10)
            };  
        }

        public async Task SendAsync(HttpRequestMessage request)
        {
            await _httpClient.SendAsync(request).ConfigureAwait(false);
        }

        public async Task<T> SendAsync<T>(HttpRequestMessage request) where T : class
        {
            var response = await _httpClient.SendAsync(request).ConfigureAwait(false);

            if (!response.IsSuccessStatusCode)
            {
                throw new Exception($"Received a {response.StatusCode} response");
            }

            if (response.StatusCode == HttpStatusCode.NoContent)
            {
                return null;
            }

            var content = response.Content != null ? await response.Content.ReadAsStringAsync().ConfigureAwait(false) : null;
            if (string.IsNullOrEmpty(content))
            {
                return null;
            }

            return JsonConvert.DeserializeObject<T>(content);
        }
    }
}
