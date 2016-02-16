import processing.serial.*; 
 
Serial myPort;    // The serial port
int[] arr = new int[4096];
int[] lin_arr;

int zoom = 1;

String s;
int maxy = 10;
void setup() { 
  size(1200,800); 
   lin_arr = new int[width+1];
frameRate(60);
  // List all the available serial ports: 
  printArray(Serial.list()); 
  // I know that the first port in the serial list on my mac 
  // is always my  Keyspan adaptor, so I open Serial.list()[0]. 
  // Open whatever port is the one you're using. 
  myPort = new Serial(this, Serial.list()[1], 256000); 
  myPort.bufferUntil('\n'); 
  
  
} 
 
void draw() { 

 background(0);
  stroke(255,255,0);
   fill(255);
   smooth();


  if (s != null) {
    println(s);
    try{
    String[] inString = match(trim(s),"\\<(.*?)\\>\\[(.*?)\\]");
    println(inString[1]);
    String[] list = split(inString[2], ",");
  for (int i=1 ;i<list.length-2;++i) arr[i] =  Integer.parseInt(trim(list[i]));
  maxy = max(arr);
  if(maxy < 10) maxy = 10;
  println(maxy);
  println("ok");
    }
    catch(Exception e) { s = null;};
    s = null;
    for(int i = 0; i < width; i++) lin_arr[i] = 0;
      for(int i = 1; i < 4095; i++) {
    int Gx = round(map(i, 1,4095,0,width));
    int Gy = round(map(arr[i], 0,maxy,0, height));
    lin_arr[Gx] += Gy;
  }
  }
  

  
  for(int i = 0; i < width; i++) line(i,height,i,height-lin_arr[i]/(1.1*4096/width));
  
} 
 
void serialEvent(Serial p) { 
s = p.readString();
// String[] inString = match(trim(s),"<(.*?)>[(.*?)]");
// for(int i = 0; i < inString.length; i++) println("tag "+inString[i]+";");
/* String[] list = split(inString[1], ",");
  for (int i=0 ;i<list.length;++i)
  {arr[i] =  Integer.parseInt(list[i]);} */
//  println(s);
  //println(inString[1]);
} 

void mouseWheel(MouseEvent event) {
  float e = event.getCount();
  println(e);
}