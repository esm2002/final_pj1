class Page {
    public:
        Page(int x_pos, int y_pos, int new_width, int new_height, int new_id, char new_content);
        int get_x(){return x;}
        int get_y(){return y;}
        int get_width(){return width;}
        int get_height(){return height;}
        int get_id(){return id;}
        char get_content(){return content;}
        bool having_overlaped_part(Page page);
        void set_x(int new_x){x=new_x;}
        void set_y(int new_y){y=new_y;}
        void set_content(char new_content){content=new_content;}
    private:
        int x, y; // position of the page on the board
        int width, height; // width and height of the page 
        int id; // unique id for each page
        char content; //not unique
};

Page::Page(int x_pos, int y_pos, int new_width, int new_height, int new_id, char new_content){
    x = x_pos;
    y = y_pos;
    width = new_width;
    height = new_height;
    id = new_id;
    content = new_content;
}

bool Page::having_overlaped_part(Page page){
  bool is_overlaped = true;

  int p_x = page.get_x();
  int p_y = page.get_y();
  int p_width = page.get_width();
  int p_height = page.get_height();

  if(((p_x+p_width-1)<x)||((x+width-1)<p_x)){
    is_overlaped = false; //not overlaped
  }
  if(((p_y+p_height-1)>y)||((y+height-1)>p_y)){
    is_overlaped = false; //not overlaped
  }
  //else, overlaped
  return is_overlaped;
}
