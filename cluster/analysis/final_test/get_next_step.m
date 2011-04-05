% used for generate next step in testing
function new_coord = get_next_step(direction, current_point, sigxy)
tx = current_point(1);
ty = current_point(2);
angle = angle_convert(direction);
step_len = 1 + unidrnd(3); % step length is from 1~4  
new_x = tx + round(step_len * cos(angle*pi/180));
new_y = ty + round(step_len * sin(angle*pi/180));
sentry_distance = inf;
%% project location to grid
for i = tx - step_len : tx + step_len
    for j = ty - step_len : ty + step_len
        if isempty(find(sigxy.x== i & sigxy.y == j, 1))
            continue; % check if the point has reading
        else
            distance = sqrt(sum(([new_x, new_y] - [i, j]) .^ 2));
            if distance < sentry_distance
                sentry_distance = distance;
                new_coord = [i, j];
            end
        end
    end
end