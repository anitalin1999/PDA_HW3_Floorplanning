//read file: https://processing.org/reference/BufferedReader.html
//save file: https://processing.org/reference/save_.html
//draw a rectangle: https://processing.org/reference/rect_.html
//add text: https://processing.org/reference/text_.html
//display text: https://processing.org/reference/textAlign_.html

// declare variables
BufferedReader reader;
String line;

void setup() {
  size(600, 600); //picture size
  background(255);
  reader = createReader("draw.floorplan");
}

void draw() {
  try {
    line = reader.readLine();
  } catch (IOException e) {
    e.printStackTrace();
    line = null;
  }
  if (line == null) {
    // Stop reading because of an error or file is empty
    noLoop();  
  } else {
    String[] pieces = split(line, TAB);
    float x = float(pieces[0]);
    float y = float(pieces[1]);
    float w = float(pieces[2]);
    float h = float(pieces[3]);

    fill(192,192,192,120);
    rect(x,y,w,h);
    
    String s = '('+pieces[0]+','+pieces[1]+')';    
    fill(0);
    textSize(8);
    textAlign(CENTER, CENTER);
    text(s,x+w/2,y+h/2);
   
    
  }   
  
  float outlineW = 454; // outline size
  float outlineH = 454;
  line(0,0,outlineW,0);
  line(outlineW,0,outlineW,outlineH);
  line(outlineW,outlineH,outlineW,0);
  line(outlineW,outlineH,0,outlineH);
  save("out.jpg");
}
