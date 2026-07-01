#include <iostream>
#include <string>
#include <cstdint>
#include <cctype>
#include <bitset>

using namespace std;

#define pos_dotpos (60ULL)
#define size_value ((1ULL << 54) - 1)
#define pos_sign (55ULL)
#define pos_valuestartpos (56ULL)
#define size_valuestartpos (0xfULL)
#define size_dotpos (0xfULL)

uint64_t stf(const string& s){
  uint64_t out = 0;
  size_t ptr = 0;

  int numbercounter = 0;
  int dotpos = -1;
  int valuestartpos = 0;

  if(s[0] == '-'){
    ptr++;
    out = 1ULL << pos_sign;
  }

  for(;ptr < s.length(); ptr++){
    char c = s[ptr];
    if(isdigit(c)){
      if(c == '0' && dotpos == numbercounter && dotpos != -1) continue;

      if(c == '0')
      {
        valuestartpos++;
        continue;
      }
      
      out = (out & ~size_value) | (((out * 10) + (c - '0')) & size_value);
      numbercounter++;
    }
    else if(c == '.'){
      dotpos = numbercounter;
    }
  }

  // set value start pos
  out |= (valuestartpos & size_valuestartpos) << pos_valuestartpos;

  dotpos = (dotpos >= 0 ? dotpos : numbercounter) & size_dotpos;
  out |= static_cast<uint64_t>(dotpos) << pos_dotpos;

  return out;
}

string fts(uint64_t n){
  string out;

  uint8_t sign = (n >> pos_sign) & 1;

  if(sign) out += '-';
  size_t dotpos = n >> pos_dotpos;
  
  uint64_t x = n & size_value;
  uint64_t r = 0;

  uint8_t valuestartpos = (n >> pos_valuestartpos) & size_valuestartpos;

  if(dotpos == 0)
    out += (sign) ? "1." : "0.";

  //get number
  while(x != 0){
    r *= 10;
    r += x % 10;
    x /= 10;
  }

  for(uint8_t counter_valuestartpos = 0; counter_valuestartpos < valuestartpos; counter_valuestartpos++){
    out += '0';
  }

  while(r != 0){
    if((out.length() - sign) == dotpos) out += '.';

    out += (r % 10) + '0';
    if(r != 0)r /= 10;
  }

  if(out[out.length() - sign] == '.')
    out += "0";
  else if((out.length() - sign) == dotpos)
    out += ".0";

  return out;
}

// fadd dst, a, b
uint64_t fadd(uint64_t a, uint64_t b){
  
}

int main(){
  uint64_t num = stf("-0.0");
  string s = fts(num);
  cout << s << endl;
  uint64_t num1 = stf(s);

  bitset<64> a(num);

  cout << a << endl;

  cout << fts(num1) << endl;
  return 0;
}
