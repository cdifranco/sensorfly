function next_step = generate_next_step(dn, u, k, x,y)
% u is the direction from 0 to 7
angle = 2*pi*u/dn + (rand*2-1)*pi/dn;

next_x = x + k*cos(angle);
next_y = y + k*sin(angle);

if next_x < 1
    if next_y > 4
        next_y = 8 - next_y;
    end
else
    if next_y > 9
        next_y = 18 - next_y;
    end  
end
if next_y < 0
    next_y = 0 - next_y;
end  
 
if next_y < 4
    if next_x < 0 
        next_x = 0 - next_x;
    elseif next_x > 3
        next_x = 6 - next_x;
    end
else
    if next_x < 1 
        next_x = 2 - next_x;
    elseif next_x > 3
        next_x = 6 - next_x;
    end
end

next_step = [next_x next_y];
