% generate base location
b = [];
for i = 1 : base_number
    rand_y = rand*9;
    if rand_y > 4
        rand_x = 1+rand*2;
    else
        rand_x = rand*3;
    end
    b = [b; i rand_x, rand_y];%real location
end
