% used for generate data set, used in generate cluster
function [new_x new_y compass_reading] = next_point(current_point, signatures)
tx = current_point(1)/10;
ty = current_point(2)/10;
reference_compass_reading = signatures(signatures(:,1) == current_point(1) & signatures(:,2) == current_point(2), 4);
isvalid_point = 0;
while ~isvalid_point
    step_len = (5+ unidrnd(4)); %step length is from 0.5m to 0.9m
    angle = (rand*2-1)*pi;
    new_x = (round(tx + step_len * cos(angle)))*10;
    new_y = (round(ty + step_len * sin(angle)))*10;
    if isempty(find(signatures(:,1)==new_x & signatures(:,2) == new_y, 1))
        continue;
    else
        isvalid_point = 1;
    end
end
compass_reading = reference_compass_reading + (angle-90)*10;
if compass_reading > 3600
    compass_reading = compass_reading - 3600;
end

%{
%% Initialization
current_x = current_point(1);
current_y = current_point(2);
reference_compass_reading = signatures(signatures(:,1) == current_x & signatures(:,2) == current_y, 4);
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
    if isempty(find(signatures(:,1)==new_x & signatures(:,2) == new_y, 1))
        continue;
    else
        isvalid_sig = 1;
    end
end
%% Get direction
u = [0 1 0];
v = [x_change y_change 0];
cos_theta = dot(u,v)/(norm(u)*norm(v));
theta = acos(cos_theta)*180/pi;
if x_change > 0
    theta = 360 - theta;
end
compass_reading = reference_compass_reading + theta*10;
if compass_reading > 3600
    compass_reading = compass_reading - 3600;
end
%}

