function [next_point compass_reading] = next_point(current_point, signature)
%% Initialization
next_point = [-1, -1];
current_x = current_point(1);
current_y = current_point(2);
reference_dir = signature(signature(1:2) == current_point, 4);
isvalid_sig = 0;

%% Get Next Point
while isvalid_sig == 0
    x_change = (unidrnd(21)-11)*10;
    y_change = (unidrnd(21)-11)*10;
    if x_change == 0 && y_change == 0
        continue; %at least move a little
    end
    new_x = current_x + x_change;
    new_y = current_y + y_change;
    if isempty(find(signature(:,1)==new_x & signature(:,2) == new_y, 1))
        continue;
    else
        isvalid_sig = 1;
    end
end
next_point = [new_x new_y];
%% Get direction
u = [0 1 0];
v = [x_change y_change 0];
cos_theta = dot(u,v)/(norm(u)*norm(v));
theta = acos(cos_theta)*180/pi;
if x_change > 0
    theta = 360 - theta;
end
compass_reading = reference_dir + theta*10;
if compass_reading > 3600
    compass_reading = compass_reading - 3600;
end
    


