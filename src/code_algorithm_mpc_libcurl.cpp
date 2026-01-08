#include <bits/stdc++.h>
#include <curl/curl.h>
#include <sys/stat.h>
using namespace std;

// Callback để ghi dữ liệu download vào buffer (cho đo bandwidth)
size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t totalSize = size * nmemb;
    string *buffer = (string *)userp;
    buffer->append((char *)contents, totalSize);
    return totalSize;
}

// Callback để ghi dữ liệu vào file
size_t WriteFileCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t totalSize = size * nmemb;
    FILE *fp = (FILE *)userp;
    return fwrite(contents, size, nmemb, fp);
}



// Hàm lấy extension từ URL hoặc Content-Type
string get_file_extension(CURL *curl, const string &url)
{
    // Thử lấy từ URL trước
    size_t last_dot = url.find_last_of(".");
    size_t last_slash = url.find_last_of("/");
    
    if (last_dot != string::npos && last_dot > last_slash)
    {
        string ext = url.substr(last_dot);
        // Loại bỏ query parameters nếu có
        size_t query_pos = ext.find("?");
        if (query_pos != string::npos)
        {
            ext = ext.substr(0, query_pos);
        }
        return ext;
    }
    
    // Nếu không có extension trong URL, thử lấy từ Content-Type
    char *content_type = nullptr;
    curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &content_type);
    
    if (content_type)
    {
        string ct(content_type);
        if (ct.find("text/plain") != string::npos) return ".txt";
        if (ct.find("text/html") != string::npos) return ".html";
        if (ct.find("application/pdf") != string::npos) return ".pdf";
        if (ct.find("image/jpeg") != string::npos) return ".jpg";
        if (ct.find("image/png") != string::npos) return ".png";
        if (ct.find("video/mp4") != string::npos) return ".mp4";
        if (ct.find("application/json") != string::npos) return ".json";
        if (ct.find("application/xml") != string::npos) return ".xml";
    }
    
    // Mặc định
    return ".dat";
}

// Hàm download file và lưu vào đĩa
bool download_and_save_file(CURL *curl, const string &url, const string &save_path, string &actual_extension)
{
    FILE *fp = fopen(save_path.c_str(), "wb");
    if (!fp)
    {
        cerr << "Cannot open file for writing: " << save_path << endl;
        return false;
    }

    curl_easy_setopt(curl, CURLOPT_BUFFERSIZE, 102400L);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "curl/7.91.0");
    curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 50L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
    curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2_0);

    // Sử dụng callback để ghi vào file
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteFileCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

    CURLcode res = curl_easy_perform(curl);
    
    fclose(fp);

    if (res != CURLE_OK)
    {
        cerr << "Download failed: " << curl_easy_strerror(res) << endl;
        remove(save_path.c_str()); // Xóa file lỗi
        return false;
    }

    // Lấy extension thực tế từ server
    actual_extension = get_file_extension(curl, url);
    
    cout << "✓ File saved to: " << save_path << endl;
    cout << "  Detected format: " << actual_extension << endl;
    return true;
}

// Hàm đo tốc độ download thực tế (giữ nguyên)
double measure_download_speed(CURL *curl, const string &url, double &chunk_size)
{
    string buffer;
    curl_easy_setopt(curl, CURLOPT_BUFFERSIZE, 102400L);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "curl/7.91.0");
    curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 50L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
    curl_easy_setopt(curl, CURLOPT_FTP_SKIP_PASV_IP, 1L);
    curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);
    curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2_0);

    auto start = chrono::high_resolution_clock::now();
    CURLcode res = curl_easy_perform(curl);
    auto end = chrono::high_resolution_clock::now();

    if (res != CURLE_OK)
    {
        cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << endl;
        return -1.0;
    }
    double download_speed_bps = 0.0;
    curl_easy_getinfo(curl, CURLINFO_SPEED_DOWNLOAD, &download_speed_bps);

    double download_time = chrono::duration<double>(end - start).count();
    chunk_size = buffer.size() / 1000.0;
    double bandwidth = (chunk_size * 8) / download_time;

    return bandwidth;
}

// Hàm tạo tất cả tổ hợp chunk
void Combine_Chunk(int P, string combo, vector<string> &CHUNK_COMBO_OPTIONS)
{
    if (combo.length() == P)
    {
        CHUNK_COMBO_OPTIONS.push_back(combo);
        return;
    }
    for (char i = '0'; i <= '2'; i++)
    {
        combo.push_back(i);
        Combine_Chunk(P, combo, CHUNK_COMBO_OPTIONS);
        combo.pop_back();
    }
}

double mpc(vector<double> &past_bandwidth,
           vector<double> &past_bandwidth_ests,
           vector<double> &past_errors,
           vector<double> &all_future_chunks_size,
           int P, 
           double buffer_size,
           int video_chunk_remain,
           int last_quality)
{
    double curr_error = 0.0;
    if (!past_bandwidth_ests.empty() && !past_bandwidth.empty())
    {
        curr_error = abs(past_bandwidth_ests.back() - past_bandwidth.back()) /
                     max(past_bandwidth.back(), 1e-6);
    }
    past_errors.push_back(curr_error);

    int use_sample = min((int)past_bandwidth.size(), 5);
    double bandwidth_sum = 0.0;
    for (int i = past_bandwidth.size() - use_sample; i < past_bandwidth.size(); i++)
    {
        bandwidth_sum += 1.0 / max(past_bandwidth[i], 1e-6);
    }
    double harmonic_bandwidth = use_sample / bandwidth_sum;

    int error_sample = min((int)past_errors.size(), 5);
    double max_error = 0.0;
    if (error_sample > 0)
    {
        auto start = past_errors.end() - error_sample;
        max_error = *max_element(start, past_errors.end());
    }

    double future_bandwidth = harmonic_bandwidth / (1.0 + max_error);
    future_bandwidth = max(future_bandwidth, 1e-6);
    past_bandwidth_ests.push_back(future_bandwidth);

    cout << "[MPC Debug] Future bandwidth estimate: " << future_bandwidth << " kbps" << endl;
    cout << "[MPC Debug] Current buffer: " << buffer_size << " seconds" << endl;

    vector<string> CHUNK_COMBO_OPTIONS;
    Combine_Chunk(P, "", CHUNK_COMBO_OPTIONS);

    double max_reward = -1e18;
    string best_combo = "";
    double start_buffer = buffer_size;

    for (const string &chunk_combo : CHUNK_COMBO_OPTIONS)
    {
        double curr_rebuffer_time = 0.0;
        double curr_buffer = start_buffer;
        double bitrate_sum = 0.0;
        double smoothness_dif = 0.0;
        int tmp_last_quality = last_quality;

        for (int chunk_index = 0; chunk_index < chunk_combo.length(); chunk_index++)
        {
            int chunk_quality = chunk_combo[chunk_index] - '0';
            int idx = (video_chunk_remain + chunk_index) * 3 + chunk_quality;

            if (idx >= all_future_chunks_size.size())
                continue;

            double download_time = all_future_chunks_size[idx] / future_bandwidth;

            if (curr_buffer < download_time)
            {
                curr_rebuffer_time += download_time - curr_buffer;
                curr_buffer = 0.0;
            }
            else
            {
                curr_buffer -= download_time;
            }

            curr_buffer += 1.0;
            bitrate_sum += (chunk_quality + 1) * 5.0;
            smoothness_dif += abs(chunk_quality - tmp_last_quality) * 5.0;

            tmp_last_quality = chunk_quality;
        }

        double reward = bitrate_sum / 1000.0 - 10.0 * curr_rebuffer_time - smoothness_dif / 1000.0;

        if (reward > max_reward)
        {
            max_reward = reward;
            best_combo = chunk_combo;
        }
    }

    cout << "[MPC Debug] Best combo: " << best_combo << " (reward: " << max_reward << ")" << endl;

    int bit_rate = last_quality;
    if (!best_combo.empty())
    {
        bit_rate = best_combo[0] - '0';
    }
    return bit_rate;
}

int main()
{
    curl_global_init(CURL_GLOBAL_DEFAULT);
    CURL *curl = curl_easy_init();

    if (!curl)
    {
        cerr << "Failed to initialize CURL" << endl;
        return 1;
    }


    string download_dir = "/home/backne/Trung/code/code_mpc/MPC_libcurl/download";

    string base_url = "https://100.66.2.42:8443/longdress_test2.bin";
    vector<string> quality_suffix = {"_low", "_medium", "_high"};
    vector<string> quality_names = {"LOW", "MEDIUM", "HIGH"};

    vector<double> past_bandwidth;
    vector<double> past_bandwidth_ests;
    vector<double> past_errors;
    vector<int> quality_history; 
    
    vector<double> all_future_chunks_size;
    for (int i = 0; i < 100; i++) {
        all_future_chunks_size.push_back(625.0);
        all_future_chunks_size.push_back(1250.0);
        all_future_chunks_size.push_back(1875.0);
    }

    int P = 5;
    double buffer_size = 1.0;
    int last_quality = 0;
    int video_chunk_remain = 0;
    int total_segments = 10;

    cout << "===== MPC Adaptive Bitrate Streaming (WITH FILE DOWNLOAD) =====" << endl;
    cout << "Download directory: " << download_dir << endl;
    cout << "Lookahead: " << P << " chunks" << endl;
    cout << "Initial buffer: " << buffer_size << " seconds" << endl;
    cout << "Starting quality: " << last_quality << " (LOW)" << endl;
    cout << "Rebuffer penalty: 10.0" << endl << endl;

    for (int seg = 0; seg < total_segments; seg++)
    {
        cout << "===== Segment " << seg + 1 << " =====" << endl;

        int next_quality;
        
        if (past_bandwidth.empty()) {
            next_quality = 0;
            cout << "First chunk: using LOW quality to measure bandwidth" << endl;
        } else {
            next_quality = (int)mpc(past_bandwidth, past_bandwidth_ests, past_errors,
                                    all_future_chunks_size, P, buffer_size,
                                    video_chunk_remain, last_quality);
            
            cout << "MPC Decision: Quality " << next_quality
                 << " (" << quality_names[next_quality] << " - " 
                 << (next_quality + 1) * 5 << " Mbps)" << endl;
        }

        // Lưu quality vào lịch sử
        quality_history.push_back(next_quality);

        cout << "Downloading: " << base_url << endl;
        
        // Đo bandwidth
        double chunk_size_kb;
        double measured_bandwidth = measure_download_speed(curl, base_url, chunk_size_kb);

        if (measured_bandwidth < 0)
        {
            cout << "Download failed" << endl;
            if (!past_bandwidth.empty()) {
                cout << "Using previous bandwidth estimate" << endl;
                measured_bandwidth = past_bandwidth.back();
            } else {
                cout << "No previous data, assuming 1000 kbps" << endl;
                measured_bandwidth = 1000.0;
            }
        }
        else
        {
            cout << "Measured bandwidth: " << measured_bandwidth << " kbps" << endl;
            cout << "Chunk size: " << chunk_size_kb << " KB" << endl;
        }
        
        past_bandwidth.push_back(measured_bandwidth);

        // Download và lưu file với tên theo segment và quality
        stringstream ss;
        ss << download_dir << "/segment_" 
           << setfill('0') << setw(3) << (seg + 1)
           << "_" << quality_names[next_quality];
        string save_path_base = ss.str();
        
        cout << "Saving file..." << endl;
        string actual_extension;
        string temp_save_path = save_path_base + ".tmp";
        
        if (download_and_save_file(curl, base_url, temp_save_path, actual_extension))
        {
            // Đổi tên file với extension đúng
            string final_save_path = save_path_base + actual_extension;
            rename(temp_save_path.c_str(), final_save_path.c_str());
            
            // Lấy kích thước file đã lưu
            struct stat st;
            if (stat(final_save_path.c_str(), &st) == 0)
            {
                cout << "File size: " << st.st_size / 1024.0 << " KB" << endl;
            }
        }

        // Cập nhật buffer
        double download_time = chunk_size_kb * 8 / measured_bandwidth;
        buffer_size = max(0.0, buffer_size - download_time) + 1.0;
        cout << "Download time: " << download_time << " seconds" << endl;
        cout << "Buffer level: " << buffer_size << " seconds" << endl;

        if (!past_bandwidth_ests.empty())
        {
            cout << "Bandwidth estimate: " << past_bandwidth_ests.back() << " kbps" << endl;
        }
        if (!past_errors.empty())
        {
            cout << "Prediction error: " << past_errors.back() * 100 << "%" << endl;
        }

        video_chunk_remain++;
        last_quality = next_quality;

        cout << endl;
    }

    curl_easy_cleanup(curl);
    curl_global_cleanup();

    cout << "===== Streaming Statistics =====" << endl;
    if (!past_bandwidth.empty()) {
        cout << "Average bandwidth: "
             << accumulate(past_bandwidth.begin(), past_bandwidth.end(), 0.0) / past_bandwidth.size()
             << " kbps" << endl;
    }
    if (!past_errors.empty()) {
        cout << "Average prediction error: "
             << accumulate(past_errors.begin(), past_errors.end(), 0.0) / past_errors.size() * 100
             << "%" << endl;
    }
    
    // Thống kê phân bố quality
    cout << "\nQuality distribution:" << endl;
    map<int, int> quality_count;
    for (int q : quality_history) {
        quality_count[q]++;
    }
    
    for (auto &pair : quality_count) {
        cout << "  " << quality_names[pair.first] << ": " 
             << pair.second << " segments (" 
             << (pair.second * 100.0 / total_segments) << "%)" << endl;
    }

    cout << "\n✓ All files saved to: " << download_dir << endl;

    return 0;
}