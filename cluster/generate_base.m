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

b(1,1:2) = [0 0];
b(2,1:2) = [3 0];
b(3,1:2) = [0 3.5];
b(4,1:2) = [3 9];
b(5,1:2) = [1 7];
b(6,1:2) = [3 7];
b(7,1:2) = [1 5];
b(8,1:2) = [3 5];
b(9,1:2) = [1.5 0];
b(10,1:2) = [1.5 9];
b(11,1:2) = [0 2];
b(12,1:2) = [3 2];