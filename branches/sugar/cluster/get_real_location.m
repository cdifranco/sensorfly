function [x y] = get_real_location(old_x, old_y)
%%
while 1
    cmd = input('command: 1--add x; 2--add y; 3--change x');
    if cmd == 1 || cmd == 2 || cmd == 3
        break;
    end
end
%%
if cmd == 1
    x = old_x +10;
    y = old_y;
elseif cmd == 2 
    y = old_y +10;
    newx = input('new x: ');
    x = newx;
else 
    newx = input('new x: ');
    x = newx;
    y = old_y;
end