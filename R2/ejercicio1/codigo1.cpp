#include <cstdio>
#include <cstdint>
#include <cstring>

// Windows stdin remains in text mode, matching the reference's CRLF handling.
class Input {
    char buffer[1 << 20];
    size_t pos=0, size=0;
public:
    int get() {
        if(pos==size) { size=std::fread(buffer,1,sizeof buffer,stdin); pos=0; }
        return pos==size ? EOF : (unsigned char)buffer[pos++];
    }
    bool line(char* out, size_t cap, size_t& len) {
        len=0; int c; bool any=false, overflow=false;
        while((c=get())!=EOF && c!='\n') {
            any=true;
            if(len+1<cap) out[len++]=(char)c; else overflow=true;
        }
        if(c==EOF && !any) return false;
        if(len && out[len-1]=='\r') --len;
        out[len]=0;
        if(overflow) len=cap;
        return true;
    }
};
static Input input;
static uint32_t count[2000000], gaps[2000000], previous[2000000];
static int64_t sum[2000000];
// Timestamp * 4 + action fits in uint32_t, including timestamp 10^9.
static bool number(const char*& p, const char* end, uint32_t max, uint32_t& v) {
    uint64_t x=0; unsigned digits=0;
    while(p<end && *p>='0' && *p<='9') {
        if(++digits>12) return false;
        x=x*10+unsigned(*p++-'0');
    }
    if(!digits || x>max) return false;
    v=(uint32_t)x; return true;
}
int main() {
    char line[256]; size_t len;
    long long t1,t2,g,n;
    if(!input.line(line,sizeof line,len) || len>=sizeof line ||
       std::sscanf(line,"%lld %lld %lld",&t1,&t2,&g)!=3) return 0;
    if(!input.line(line,sizeof line,len) || len>=sizeof line ||
       std::sscanf(line,"%lld",&n)!=1) return 0;
    for(long long i=0;i<n && input.line(line,sizeof line,len);++i) {
        if(len>=sizeof line) continue;
        const char* p=line; const char* end=line+len; uint32_t t,u;
        if(!number(p,end,1000000000,t) || p==end || *p++!=' ' ||
           !number(p,end,1999999,u) || p==end || *p++!=' ') continue;
        unsigned a;
        size_t remaining=(size_t)(end-p);
        if(remaining==5 && !std::memcmp(p,"LOGIN",5)) a=0;
        else if(remaining==4 && !std::memcmp(p,"READ",4)) a=1;
        else if(remaining==5 && !std::memcmp(p,"WRITE",5)) a=2;
        else if(remaining==6 && !std::memcmp(p,"LOGOUT",6)) a=3;
        else continue;
        if(t<t1 || t>t2) continue;
        uint32_t packed=t*4+a;
        if(count[u]) {
            if(previous[u]==packed) continue;
            int64_t delta=(int64_t)t-(previous[u]/4);
            if(delta<=g) { sum[u]+=delta; ++gaps[u]; }
        }
        previous[u]=packed; ++count[u];
    }
    int top[5]={-1,-1,-1,-1,-1};
    for(int u=0;u<2000000;++u) if(count[u]) {
        for(int j=0;j<5;++j) if(top[j]<0 || count[u]>count[top[j]]) {
            for(int k=4;k>j;--k) top[k]=top[k-1];
            top[j]=u; break;
        }
    }
    if(top[0]<0) std::puts("SIN DATOS");
    for(int u:top) if(u>=0) {
        if(!gaps[u]) std::printf("%d %u NA\n",u,count[u]);
        else {
            int64_t rounded=(int64_t)((double)sum[u]/gaps[u]*1000.0+0.5);
            std::printf("%d %u %lld.%03lld\n",u,count[u],
                        (long long)(rounded/1000),(long long)(rounded%1000));
        }
    }
}
