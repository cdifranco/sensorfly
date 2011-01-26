function [x y] = get_real_location(old_x, old_y)
%%
while 1
    cmd = input('command: 1--add x; 2--add y');
    if cmd ==1 || cmd ==2
        break;
    end
end
%%
if cmd == 1
    x = old_x +10;
    y = old_y;
else 
    y = old_y +10;
    newx = input('new x: ');
    x = newx;
end