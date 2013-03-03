module arm(thickness, length, width, slot_length, slot_width) {
	difference(){
		cube([length, width, thickness], center = true);
		linear_extrude(h = thickness+1, center = true){
			hull(){
				translate([slot_length/2,0])
					circle(r = slot_width/2);
				translate([-slot_length/2,0])
					circle(r = slot_width/2);
			}
		}
	}
}

module annulus(thickness, r1, r2){
	difference(){
		cylinder(r = r2, h = thickness, center = true);
		cylinder(r = r1, h = thickness+1, center = true);
	}
}

module slider(thickness, width, length) {
//		linear_extrude(h = thickness, center = true){
			hull(){
				translate([length/2,0])
					circle(r = width/2);
				translate([-length/2,0])
					circle(r = width/2);
			}
//		}
}

color("Blue",1)
arm(5, 1000, 100, 900, 20);

translate([0,0,5.      1])
	annulus(5, 100, 120);

slider(0, 20, 220);