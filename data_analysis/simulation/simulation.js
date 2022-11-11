let coords;
let i = 0;
let last_x = null;
let last_y = null;
let colours = ['pink', 'green', 'red', 'blue', 'orange', 'purple', 'black', 'grey'];
function setup() {
  coords = loadJSON('./data.json');
  createCanvas(1000, 1000);
}

function draw() {
  clear();
  translate(500, 500);
  col = 0;
  Object.keys(coords).forEach((key, idx)=>{
    coord = coords[key];
    if(i >= idx) {
      if (coord.t == "o") {
        stroke(colours[col]);
        fill(colours[col]);
        ellipse(coord.x*4, coord.y*4, 10, 10);
        if(last_x != null) {
          strokeWeight(2);
          line(last_x, last_y, coord.x*4, coord.y*4);
          strokeWeight(1);
        }
        last_x = coord.x*4;
        last_y = coord.y*4;
        col += 1;
      } else {
        if (i == idx) {
          drawingContext.setLineDash([10, 10]);
          line(last_x, last_y, coord.x*4, coord.y*4);
          drawingContext.setLineDash([]);
        }
        ellipse(coord.x*4, coord.y*4, 5, 5);
      }
    }
  });
  i += 1;
  if(col == 8) {
    col = 0;
    i = 0;
  }
}
