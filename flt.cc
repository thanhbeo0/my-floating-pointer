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

// cấu trúc: [dotpos: 64...60bit][valuestartpos: 60...56bit][sign: 55bit][value: 54...0bit]

uint64_t stf(const string& s){
  uint64_t out = 0;
  size_t ptr = 0;

  int numbercounter = 0;
  int dotpos = -1;
  int valuestartpos = 0;

  // kiểm tra dấu và thêm dấu
  if(s[0] == '-'){
    ptr++;
    out = 1ULL << pos_sign;
  }

  for(;ptr < s.length(); ptr++){
    char c = s[ptr];
    if(isdigit(c)){
      if(
        c == '0' && // kiểm tra xem c có phải không không
        dotpos == numbercounter && // kiểm tra xem vị trí dấu chấm có bằng tiếp số lượng số valid không
        dotpos <= -1 // kiểm tra xem vị trí dấu phẩy đã có dấu phẩy chưa
      ) continue;

      // xử lý bù không sau dấu phẩy
      if(c == '0')
      {
        // tăng bù không để xử lý trường hợp '0.00000';
        valuestartpos++;

        // bỏ qua đoạn này
        continue;
      }
      
      out =
      (out & ~size_value) | // đảm bảo phần bit [dotpos][valuestartpos][sign] trước khi thêm number vào value
      (((out * 10) + (c - '0')) & size_value); // thêm number vào value
      
      // khi đọc một con số thì tăng nó lên
      numbercounter++;
    }
    else if(c == '.'){
      // chỉnh dotpos bằng đúng ký tự number đã đọc
      dotpos = numbercounter;
    }
  }

  /*
  xử lý và thêm size_valuestartpos
  và thêm vào vùng [dotpos][valuestartpos][sign][value]
  */
  out |= (valuestartpos & size_valuestartpos) << pos_valuestartpos;

  /*
  xử lý kết quả vị trí dấu chấm, nếu nó >= 0 tức là nó đã được tạo
  thì cho nó bằng chính nó.
  Còn nếu nó không >= thì cho vị trí dấu chấm ở cuối mảnh
  */
  dotpos = (dotpos >= 0 ? dotpos : numbercounter) & size_dotpos;

  // thêm vào vùng [dotpos][valuestartpos][sign][value]
  out |= static_cast<uint64_t>(dotpos) << pos_dotpos;

  return out;
}

string fts(uint64_t n){
  string out;

  // lấy bit dấu
  uint8_t sign = (n >> pos_sign) & 1;

  // kiểm tra dấu và thêm dấu trừ vào
  if(sign) out += '-';
  size_t dotpos = n >> pos_dotpos;
  
  // lấy value
  uint64_t x = n & size_value;
  uint64_t r = 0;

  // lấy bù không
  uint8_t valuestartpos = (n >> pos_valuestartpos) & size_valuestartpos;

  // kiểm tra xem dotpos có ở đầu tiên không
  if(dotpos == 0)
    out += "0.";

  //duyệt lấy các số trong value
  while(x != 0){
    r *= 10;
    r += x % 10;
    x /= 10;
  }

  // bù thêm 0 với số lương valuestartpos
  for(uint8_t counter_valuestartpos = 0; counter_valuestartpos < valuestartpos; counter_valuestartpos++){
    out += '0';
  }

  // duyệt lại các số đã được đảo lần nữa
  while(r != 0){
    if((out.length() - sign) == dotpos) out += '.';

    out += (r % 10) + '0';
    if(r != 0)r /= 10;
  }

  // bổ sung thêm số không nếu nó kết bằng dấu chấm
  if(out[out.length() - sign] == '.')
    out += "0";

    // bổ sung thêm dấu chấm nếu nó nằm ngoài số hoặc không được thêm vào
  if((out.length() - sign) == dotpos)
    out += ".0";

  // trả về out
  return out;
}

// fadd dst, a, b
uint64_t fadd(uint64_t a, uint64_t b){
  // kết quả
  uint64_t res = 0;

  // giá trị thực của float
  uint64_t valueA = a & size_value;

  // vị trí dấu chấm
  uint8_t dotposA = (a >> pos_dotpos) & size_dotpos;

  // phần bù thêm 0
  uint8_t valuestartposA = (a >> pos_valuestartpos) & size_valuestartpos;

  // lấy số trước dấu phẩy
  uint32_t integervalueA = valueA >> (dotposA+1);

  // lấy số sau dấu phẩy
  uint32_t mantissavalueA = valueA & size_valuestartpos;

  // kiểm tra
  cout << "integer: " << integervalueA << '\n'
  << "mantissa: " << mantissavalueA << '\n'
  << "value: " << (mantissavalueA | (integervalueA << (dotposA+1))) << endl;
  return res;
}

int main(){
  uint64_t num = stf("-1.0");
  string s = fts(num);
  cout << s << endl;
  uint64_t num1 = stf(s);

  bitset<64> a(num);

  cout << a << endl;

  cout << fts(num1) << endl;

  fadd(num,num);
  return 0;
}
