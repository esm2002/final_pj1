#include <fstream>
#include <vector>
#include <algorithm>

#include "page.h"

using std::vector;
using std::endl;
using std::ofstream;

class Board {
    public:
        Board(int num_jobs, int width, int height, ofstream& output_stream);
        ~Board();

        void print_board();
        void print_job(int job_idx, char job_type, int id);

        //job functions
        void insert_page(int x, int y, int width, int height, int id, char content);
        void delete_page(int id);
        void modify_content(int id, char content);
        void modify_position(int id, int x, int y);

        //additional functions
        int find_idx(int id);
        bool is_above(int id1, int id2);
        void print_process();
        int num_upper(int id);
        vector<int> vec_upper(int id);
        void delete_byid(int id);
        void insert_byid(int id);
        void del_loot(vector<int>& ids);
        void ins_loot(vector<int>& ids);
        struct page_id{
          int ids;
          Page pages;
        };

    private:
        int num_jobs, width, height; 
        ofstream& output; 
        char* board; 
        vector<page_id> real_time; //각 과정에서 남아있는 page의 정보를 저장
        vector<page_id> history; //한번 insert된 page의 모든 정보를 저장
};

Board::Board(int num_jobs, int width, int height, ofstream& output_stream): output(output_stream) {
    this->width = width; 
    this->height = height;
    this->num_jobs = num_jobs; 

    board = new char[width*height]; 
  
    for (int h = 0; h < height; h++) {
        for (int w = 0; w < width; w++) {
            board[h*width + w] = ' '; //width는 상수
        }
    }
}

Board::~Board() {
    delete board;
}


void Board::print_board() { //every modification마다 불려와야 함

    int h, w;
    for (w = 0; w < width+2; w++) output << "- ";
    output << endl;
    
    for (h = 0; h < height; h++) {
        output << "| ";
        for (w = 0; w < width; w++) {
            output << board[h*width + w] << " ";
        }
        output << "| " << endl;
    }

    for (w = 0; w < width+2; w++) output << "- ";
    output << endl;
}

void Board::print_job(int job_idx, char job_type, int id) {
    output << ">> (" << job_idx <<") ";
    switch(job_type) {
        
        case 'i':
            output << "Insert ";
            break;
        case 'd':
            output << "Delete ";
            break;
        case 'm':
            output << "Modify ";
            break;
    }

    output << id << endl;
}

//additional function
int Board::find_idx(int id){ //history에서 id로 index찾기
  int idx = -1;
  for(int i=0;i<history.size();i++){
    if(history[i].ids==id){
      idx = i;
      break;
    }
  }
  return idx;
}

bool Board::is_above(int id1, int id2){ //id2가 id1 바로 위에 있는지 조사
  bool above_ck = true;
  const int idx1 = find_idx(id1);
  const int idx2 = find_idx(id2);

  Page page1 = history[idx1].pages;
  Page page2 = history[idx2].pages;
  
  if(page2.having_overlaped_part(page1)){ //겹치는 부분이 존재함

    if((idx2-idx1)==1){ //page2가 page1 바로 위에 있음
      above_ck = true;
    }
    else if(idx2<=idx1){ //동일한 page로 비교하는 경우 false return
      above_ck = false;
    }
    else{ //page1과 page2 사이에 있는 page 조사

      for(int i=(idx2-1);i>idx1;i--){
        Page pagei = history[i].pages;
        if(page1.having_overlaped_part(pagei)&&page2.having_overlaped_part(pagei)){
          above_ck = false; //page1과 page2 사이에 insert된 page 중 두 page에 동시에 겹치는 page가 있다면 false
          break;
        }
      }

    }
  }
  else{ //page1과 page2가 겹치는 부분이 없음
    above_ck = false;
  }

  return above_ck;
}

void Board::print_process(){
  for (int bh = 0; bh < height; bh++) {
        for (int bw = 0; bw < width; bw++) {
            board[bh*width + bw] = ' '; //width는 상수
        }
    }

  for(int i=0;i<real_time.size();i++){

    int p_x = (real_time[i].pages).get_x();
    int p_y = (real_time[i].pages).get_y();
    int p_height = (real_time[i].pages).get_height();
    int p_width = (real_time[i].pages).get_width();
    char p_content = (real_time[i].pages).get_content();
    
    for (int h = p_y; h < (p_y+p_height); h++){
      for (int w = p_x; w < (p_x+p_width); w++){
        board[h*width + w] = p_content; //real_time에 있는 page들을 board에 나타냄
      }
    }

  }
  output << "real_time: ";
  for(int k=0;k<real_time.size();k++){
    output << real_time[k].ids << " ";
  }
  output<<endl;

  output << "history: ";
  for(int l=0;l<history.size();l++){
    output << history[l].ids << " ";
  }
  output<<endl;

  print_board();
}

int Board::num_upper(int id){
  int num = 0;
  int idx = find_idx(id);
  for(int n=idx;n<history.size();n++){
    if(is_above(id, history[n].ids)){
      num++;
    } 
  }
  return num;
}

vector<int> Board::vec_upper(int id){
  vector<int> above_ids;

  int idx = find_idx(id);
  for(int n=idx;n<history.size();n++){
    if(is_above(id, history[n].ids)){
      above_ids.push_back(history[n].ids); //target page의 바로 위에 있는 page id 저장
    } 
  }
  std::sort(above_ids.begin(), above_ids.end()); //id 크기 오름차순 정렬
  
  return above_ids;
}

void Board::delete_byid(int id){

  for(int i=0;i<real_time.size();i++){
    if(id==real_time[i].ids){
      real_time.erase(real_time.begin()+i);
      print_process();
      break;
    }
  }

}

void Board::insert_byid(int id){
  bool is_there = false;
  int idx = find_idx(id);
  for(int j=0;j<real_time.size();j++){
    if(id==real_time[j].ids){
      is_there = true;
    }
  }

  if(!is_there){ //real_time에 없다면
    real_time.push_back(history[idx]); //history에서 struct 찾아서 push_back
    print_process();
  }
}

void Board::del_loot(vector<int>& ids){
  if(!ids.empty()){
    vector<int> next_ids = vec_upper(ids.back());
    del_loot(next_ids);
    
    delete_byid(ids.back());
    ids.erase(ids.end());
    del_loot(ids);
    return;
  }
  else{
    return;
  }
}

void Board::ins_loot(vector<int>& ids){
  if(!ids.empty()){
    int for_next = ids.front();
    insert_byid(for_next);
    ids.erase(ids.begin());
    ins_loot(ids);

    vector<int> next_ids = vec_upper(for_next);
    ins_loot(next_ids);
    return;
  }
  else{
    return;
  }
}

void Board::insert_page(int x, int y, int width, int height, int id, char content) {
    Page new_page(x,y,width,height,id,content);
    page_id new_struct = {id, new_page};
    real_time.push_back(new_struct); //new struct를 vector에 저장
    history.push_back(new_struct); //new struct를 vector에 저장
    print_process();
}

void Board::delete_page(int id) {

  vector<int> loots;
  loots.push_back(id);
  del_loot(loots);
  
  delete_byid(id); //real_time에서 page delete
  const int idx = find_idx(id);
  history.erase(history.begin()+idx);//history에서 page delete

  ins_loot(loots);
}

void Board::modify_content(int id, char content) {
  vector<int> loots;
  loots.push_back(id);
  del_loot(loots);

  delete_byid(id); //real_time에서 page delete

  const int idx = find_idx(id);
  Page* ptr = &(history[idx].pages);
  (*ptr).set_content(content);//history에서 찾은 page의 content modify

  insert_byid(id); //real_time에 page insert

  ins_loot(loots);
}

void Board::modify_position(int id, int x, int y) {
  vector<int> loots;
  loots.push_back(id);
  del_loot(loots);
  
  delete_byid(id); //real_time에서 page delete

  const int idx = find_idx(id);
  Page* ptr = &(history[idx].pages);
  (*ptr).set_x(x);
  (*ptr).set_y(y);//history에서 찾은 page의 content modify

  insert_byid(id); //real_time에 page insert

  ins_loot(loots);
}
