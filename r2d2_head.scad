// R2-D2 Retro Monitor Head - OpenSCAD
// Dimensiuni estimate pentru ecran HDMI de 7 inch
screen_width = 165;   // Lățimea ecranului (mm)
screen_height = 100;  // Înălțimea ecranului (mm)
screen_depth = 12;    // Grosimea ecranului cu tot cu placă (mm)

// Setările carcasei
wall_thickness = 4;   // Grosimea peretelui printat (mm) - 4mm e foarte solid
case_width = screen_width + (wall_thickness * 2) + 10; // Adăugăm o margine retro (bezel)
case_height = screen_height + (wall_thickness * 2) + 10;
case_depth = 55;      // Suficient de adânc pentru a ascunde telefonul Huawei în spatele ecranului

module retro_monitor() {
    difference() {
        // 1. Carcasa exterioară solidă
        cube([case_width, case_height, case_depth], center=true);
        
        // 2. Decupajul frontal prin care vom vedea ecranul
        // Mutat spre fața carcasei
        translate([0, 0, (case_depth/2) - (screen_depth/2) + 1])
            cube([screen_width, screen_height, screen_depth + 2], center=true);
            
        // 3. Scobitura masivă interioară pentru telefonul Huawei, adaptor și cabluri
        translate([0, 0, -wall_thickness])
            cube([case_width - wall_thickness*2, case_height - wall_thickness*2, case_depth], center=true);
            
        // 4. Gaură în spate pentru ieșirea cablurilor (alimentare ecran + alimentare telefon)
        translate([0, 0, -case_depth/2])
            cube([30, 20, 10], center=true);
    }
    
    // Gulerul de jos (gâtul) care se va conecta de corpul robotului
    translate([0, -(case_height/2) - 5, 0])
        rotate([90, 0, 0])
        cylinder(h=15, r=20, center=true, $fn=50);
}

// Apelăm funcția de desenare
retro_monitor();
