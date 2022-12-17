#include <iostream>
#include <cstring>
#include <unordered_map>

#define CR '\r'
#define LF '\n'
#define CNT_C (buf_[cnt])
#define EAT(c) if (!eat(c)) { return nullptr; }
#define RESET_LOCAL localcnt = 0;

namespace httpParser {

enum class M {
    GET,
    PUT,
    POST,
    DELETE,
    HEAD   
};

struct Request {
    M method_;
    int num_headers_;
    // hash table containing const char* of the header and value
    std::unordered_map<const char*, const char*> headers_;
}

struct Response {
    M method_;
    int num_headers_;
    // hash table containing const char* of the header and value
    std::unordered_map<const char*, const char*>& headers_;
}

enum class RCode {
    SUCCESS = 0;
    FAILURE = 1;
    INCOMPLETE = 2;
}
class Parser {
private:
    std::string buf_;
    std::size_t bufsize_;
    Response resp_;
    Request req_;
private:
    std::size_t cnt; // current
    std::size_t localcnt; // local cnt to construct single pieces
public:
    Parser(const char* buffer, std::size_t bufsize): buf_(buffer), bufsize_(bufsize), cnt(0), srt(0) { }
    RCode parse_request() const noexcept;
    RCode parse_response() const noexcept;
    RCode parse_headers() const noexcept;
private:
    std::string parse_method() const noexcept;
    std::string parse_url() const noexcept;
    std::string parse_version() const noexcept;
    std::string parse_scheme() const noexcept;
    std::string parse_user_info() const noexcept;
    std::string parse_url_host() const noexcept;
    std::string parse_ip() const noexcept;
private:
    [[nodiscard]] bool match(std::size_t off, char c) const noexcept;
    [[nodiscard]] bool match(char c) const noexcept;
    void set_header_resp_(const char* name, const char* value);
    void set_header_req_(const char* name, const char* value);
    void adv(std::size_t off) { cnt += off; }
};

/*********************************
 * HELPERS
 * ******************************/

const char* Parser::parse_version() const noexcept {
    // compare "HTTP" using simd instructions
    // then parse the numbers
    if (!strcmp(buf_, "HTTP"));
}

bool Parser::match(char c) const noexcept {
    return match(1, c);
}

bool Parser::match(std::size_t offset, char c) const noexcept {
    return buf_[cnt + offset] == c;
}

static bool is_unreserved(char c) {
    return c == '-' || c == '.' || c == '_' || c == '~' || std::isalnum(c);
}

static bool is_gen_delims(char c) {
    return c == ':' || c == '/' || c == '?' || c == '#' || c == '[' || c == ']' || c == "@";
}

static bool is_sub_delims(char c) {
    return ( c == '!' || c == '$' || c == '&' || c == '\'' ||
        c == '(' || c == ')' || c == '*' || c == '+' || c == ',' || c == ';' || c == '=');
}

/*******************************************************/
/*******************************************************/

bool Parser::eat(char c) {
    if (CNT_C == c) {
        adv(1);
        return true;
    }
    return false;
}

#define CHECK_NULL(var) if (var == nullptr) return nullptr;

std::string Parser::parse_request() const noexcept {
    // GET POST PUT HEAD OPTIONS DELETE
    std::string method = parse_method();
    EAT(' ');

    // HTTP, HTTPS ...
    std::strint url = parse_url();
    if (url == nullptr) return nullptr; 
}

std::string Parser::parse_method() const noexcept {
    // GET HEAD POST PUT DELETE
    switch(CNT_C) {
        case "G": {
            if (match("E") && match("T")) {
                adv(3);
                return "GET";
            }
            return nullptr;
        } 
        case "P": {
            if (match("U") && match("T")){
                adv(3);
                return "PUT";
            }
            else if (match("O") && match("S") && match("T")) {
                adv(4);
                return "POST"
            }
            return nullptr;
        } 
        case "H": {
            if (match("E") && match("A") && match("D")) {
                adv(4);
                return "HEAD";
            }
            return nullptr;
        }
        case "D": {
            if (match("E") && match("L") && match("E") && match("T") && match("E")) {
                adv(6);
                return "DELETE";
            }
            return nullptr;
        }
        default: return nullptr;
    }
    return nullptr;
}

#define ADV_LOCAL localcnt++

std::string Parser::parse_scheme() const noexcept {
    std::string scheme(50);
    for (;;) {
        switch(CNT_C) {
            case 'h': scheme[localcnt] = 'h'; ADV_LOCAL; adv(1); break;
            case 't': scheme[localcnt] = 't'; ADV_LOCAL; adv(1); break;
            case 'p': scheme[localcnt] = 'p'; ADV_LOCAL; adv(1); break;
            case 's': scheme[localcnt] = 'p'; ADV_LOCAL; adv(1); break;
            case '+': scheme[localcnt] = '+'; ADV_LOCAL; adv(1); break;
            case '-': scheme[localcnt] = '-'; ADV_LOCAL; adv(1); break;
            case '.': scheme[localcnt] = '.'; ADV_LOCAL; adv(1); break;
            default: {
                if (std::isalnum(CNT_C)) scheme[local] = CNT_C; localcnt++; adv(1); break;
                return nullptr;
            }
        }
    }
    RESET_LOCAL;
    return scheme;
}


std::string Parser::parse_url() const noexcept {
    std::string url(2000);
    std::string scheme = parse_scheme()
    EAT(':');
    if (CNT_C == '/' && match('/')) {
        adv(2);
        std::string authority = parse_authority();
        CHECK_NULL(authority);
        
    }
    RESET_LOCAL;
}

std::string Parser::parse_authority() {
    std::string authority(50);
    // add support for hex encoding
    if (is_unreserved(CNT_C) || is_sub_delims(CNT_C)) {
        std::string userinfo = parse_userinfo();
        CHECK_NULL(userinfo);
    }

    std::string host = parse_host();
    CHECK_NULL(host);
    if (CNT_C == ':')  {
        std::string port = scan_port();
    }
}

std::string Parser::scan_port() {
    std::string port(5);
    switch(CNT_C) {
        case '1': port[localcnt]; adv(1); break;
        case '2': port[localcnt]; adv(1); break;
        case '3': port[localnt]; adv(1); break;
        case '4': port[localnt]; adv(1); break;
        case '5': port[localnt]; adv(1); break;
        case '6': port[localnt]; adv(1); break;
        case '7': port[localnt]; adv(1); break;
        case '8': port[localnt]; adv(1); break;
        case '9': port[localnt]; adv(1); break;
        case '0': port[localnt]; adv(1); break;
        default: return nullptr;
    }
    RESET_LOCAL;
    return port;
}

std::string Parser::parse_userinfo() const noexcept {
    // parse ipv4 or ipv6
    std::string userinfo(50);
    while (is_unreserved(CNT_C) || is_sub_delims(CNT_C) || CNT_C == ':') {
        userinfo[localcnt] = CNT_C;
        ADV_LOCAL;
    }
    EAT('@');
    RESET_LOCAL;
    return userinfo;
}

std::string Parser::parse_host() const noexcept {
    switch(CNT_C) {
        case '[': adv(1); return parse_ip_literal();
        case '1': return parse_ipv4();
        case '2': return parse_ipv4();
        case '3': return parse_ipv4();
        case '4': return parse_ipv4();
        case '5': return parse_ipv4();
        case '6': return parse_ipv4();
        case '7': return parse_ipv4();
        case '8': return parse_ipv4();
        case '9': return parse_ipv4();
        case '0': return parse_ipv4();
    }
}

std::string Parser::parse_ip_literal() const noexcept {
}

std::string Parser::parse_ipv4() const noexcept {
    std::string ip(15);
    ip[localcnt] = CNT_C;
    adv(1);
    for (;;) {
        case '.': ip[localcnt] = '.'; adv(1); break;
        case '0': ip[localcnt] = '0'; adv(1); break;
        case '1': ip[localcnt] = '1'; adv(1); break;
        case '2': ip[localcnt] = '2'; adv(1); break;
        case '3': ip[localcnt] = '3'; adv(1); break;
        case '4': ip[localcnt] = '4'; adv(1); break;
        case '5': ip[localcnt] = '5'; adv(1); break;
        case '6': ip[localcnt] = '6'; adv(1); break;
        case '7': ip[localcnt] = '7'; adv(1); break;
        case '8': ip[localcnt] = '8'; adv(1); break;
        case '9': ip[localcnt] = '9'; adv(1); break;
        case ' ': adv(1); return ip;
        //case '%' : decode();
        default: return nullptr;
    }
}


}// httpParser