/*
 * AquaBox
 * Copyright (C) 2019  Eduard Sargsyan
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "http_helpers.h"
#include "log.h"

using namespace aquabox;

void HttpHelpers::serveStream(
    Web::Response response, const boost::filesystem::path& path, std::shared_ptr<std::ifstream> ifs, size_t length, bool removeWhenFinished)
{
    std::vector<char> buffer(131072); // Safe when server is running on one thread
    std::streamsize read_length;
    if ((read_length = ifs->read(buffer.data(), static_cast<std::streamsize>(buffer.size())).gcount()) > 0) {
        length -= read_length;
        response->write(&buffer[0], read_length);
        if (read_length == static_cast<std::streamsize>(buffer.size())) {
            response->send([response, ifs, path, length, removeWhenFinished](const SimpleWeb::error_code& ec) {
                if (!ec)
                    HttpHelpers::serveStream(response, path, ifs, length, removeWhenFinished);
                else
                    LOG_DEBUG << "Connection interrupted while sending file ";
            });
        }
    }

    if (length == 0 && removeWhenFinished) {
        std::remove(path.c_str());
    }
}

void HttpHelpers::serveFile(Web::Response response, const boost::filesystem::path& path, Web::Headers& headers, bool removeWhenFinished)
{
    static const std::unordered_map<std::string, std::string> ext2ContentType({{".js", "application/javascript"},
                                                                               {".ogg", "application/ogg"},
                                                                               {".pdf", "application/pdf"},
                                                                               {".xhtml", "application/xhtml+xml"},
                                                                               {".json", "application/json"},
                                                                               {".zip", "application/zip"},
                                                                               {".gif", "image/gif"},
                                                                               {".jpeg", "image/jpeg"},
                                                                               {".jpg", "image/jpeg"},
                                                                               {".png", "image/png"},
                                                                               {".tif", "image/tiff"},
                                                                               {".tiff", "image/tiff"},
                                                                               {".ico", "image/x-icon"},
                                                                               {".djvu", "image/vnd.djvu"},
                                                                               {".svg", "image/svg+xml"},
                                                                               {".css", "text/css"},
                                                                               {".csv", "text/csv"},
                                                                               {".html", "text/html"},
                                                                               {".htm", "text/html"},
                                                                               {".txt", "text/plain"},
                                                                               {".xml", "text/xml"}});

    auto ifs = std::make_shared<std::ifstream>(path.c_str(), std::ifstream::in | std::ios::binary | std::ios::ate);
    if (ifs->bad()) {
        LOG_DEBUG << "File '" << path << "'can't be found/opened so send over http";
        response->write(SimpleWeb::StatusCode::client_error_not_found, headers);
        return;
    }

    auto length = ifs->tellg();
    ifs->seekg(0, std::ios::beg);

    auto mime = ext2ContentType.find(path.extension().string());

    if (mime == ext2ContentType.end()) {
        headers.find("Content-Type")->second = "application/octet-stream";
    }
    else {
        headers.find("Content-Type")->second = mime->second;
    }

    headers.emplace("Content-Length", std::to_string(length));
    response->write(headers);
    serveStream(response, path, ifs, length, removeWhenFinished);
}