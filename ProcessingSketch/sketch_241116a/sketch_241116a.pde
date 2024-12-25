void settings(){
  size(1200,600);
  
}

Grid g1, g2;

void setup(){
  frameRate(2);
  //noLoop();
  textAlign(CENTER, CENTER);
  g2 = new Grid(5,500);
  g1 = new Grid(g2.GridSize+2,500);
  
  
  
  g2.Gen();
  g1.Link(g2);
  
  g2.GridArray[0][0].active = 1; 
  
}

int x = 0;
int y = 0;

void draw(){
  background(0);
  translate(50,50);
  g1.render();
  translate(600,0);
  g2.render();
  
  g2.GridArray[x][y].active = 0;
  
  x++;
  x %= g2.GridSize;
  if(x==0){
    y++;
  }
  y %= g2.GridSize;
  
  g2.GridArray[x][y].active = 1;
  
}

boolean running = true;
public void keyPressed(){
  if (key == ' '){
    println("pressed");
    running = !running;
    if(running == false){
      noLoop();
      return;
    }
    loop();
    
    
  }
}

public class Box{
  int active;
  String text;
  public Box(){
  
  }
  
}

public class Grid{
  int GridSize;
  int maxSize;
  Box GridArray[][];
  public Grid(int GridSize, int maxSize){
    this.GridSize = GridSize;
    GridArray = new Box[GridSize][GridSize];
    this.maxSize = maxSize;
  }
  
  public void Link(Grid g){
    for(int x = 1; x < GridSize-1;x++){
      for(int y = 1; y < GridSize-1;y++){
        GridArray[x][y] = g.GridArray[x-1][y-1];
      }
    }   
    for(int x = 1; x < GridSize-1;x++){
      GridArray[x][0] = g.GridArray[x-1][g.GridSize-1];
      GridArray[x][GridSize-1] = g.GridArray[x-1][0];
    }
    for(int y = 1; y < GridSize-1;y++){
      GridArray[0][y] = g.GridArray[g.GridSize-1][y-1];
      GridArray[GridSize-1][y] = g.GridArray[0][y-1];
    }
    
    for(int x = 0; x <= 1; x++){
        for(int y = 0; y <= 1; y++){
            GridArray[x*(GridSize-1)][y*(GridSize-1)] = g.GridArray[(x^1)*(g.GridSize-1)][(y^1)*(g.GridSize-1)];
        }
    }
  }
  
  public void Gen(){
    for(int x = 0; x < GridSize;x++){
      for(int y = 0; y < GridSize;y++){
        GridArray[x][y] = new Box();
        GridArray[x][y].text = ""+x+":"+y;
      }
    }
  }
  
  public void render(){
    int boxSize = maxSize/GridSize;
    textSize(boxSize/3);
    strokeWeight(3);
    stroke(255,255,255);
    for(int x = 0; x < GridSize;x++){
      for(int y = 0; y < GridSize;y++){
        if(GridArray[x][y].active == 1){
          fill(255,0,0);
        }else if(GridArray[x][y].active == 2){
          fill(255,0,255);
          GridArray[x][y].active = 0;
        }else{
          fill(0,0,0);
        }
        rect(x*boxSize,y*boxSize,boxSize, boxSize);
        
        fill(0,255,0);
        text(""+x+":"+y,x*boxSize + boxSize/2,y*boxSize + boxSize/4);
        fill(0,255,255);
        text(GridArray[x][y].text,x*boxSize + boxSize/2,y*boxSize + 3*boxSize/4);
        
      }  
    }
  }
}
