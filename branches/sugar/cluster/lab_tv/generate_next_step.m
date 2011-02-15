function [next_step bumping] = generate_next_step(dn,drange, u, k, x, y, room)
%%
angle = 2*pi*u/dn + (rand*2-1)*pi/drange;
next_x = x + k*cos(angle);
next_y = y + k*sin(angle);
next_step = [next_x next_y];
intersection = [];
bumping = 0;
%% 
for i = 1 :size(room,1)
   if mod(room(i,5),2) == 0
       if next_x-room(i,1) == 0 && next_y >min(room(i,2),room(i,4)) && next_y < max(room(i,2),room(i,4))
            if room(i,5) == 2
                next_x = next_x - 0.0001;
            else
                next_x = next_x + 0.0001;
            end
       end
   else
        if next_y-room(i,2) == 0 && next_y >min(room(i,1),room(i,3)) && next_y < max(room(i,1),room(i,3))
            if room(i,5) == 1
                next_y = next_y - 0.0001;
            else
                next_y = next_y + 0.0001;
            end
        end
   end
end
%%
for i =  1 : size(room,1)
    if mod(room(i,5),2) == 0
        if (x-room(i,1))*(next_x-room(i,1)) < 0 && interp1([x next_x],[y next_y],room(i,1))>min(room(i,2),room(i,4)) && interp1([x next_x],[y next_y],room(i,1))<max(room(i,2),room(i,4))
            intersection = [intersection; sum(([room(i,1) interp1([x next_x],[y next_y],room(i,1))]-[x y]).^2).^.5  room(i,1)  interp1([x next_x],[y next_y],room(i,1))];
        else 
            intersection = [intersection; inf 0 0];
        end
    else 
       if (y-room(i,2))*(next_y-room(i,2)) < 0  && interp1([y next_y],[x next_x],room(i,2))>min(room(i,1),room(i,3)) && interp1([y next_y],[x next_x],room(i,2))<max(room(i,1),room(i,3))
            intersection = [intersection; sum(([interp1([y next_y],[x next_x],room(i,2)) room(i,2)]-[x y]).^2).^.5  interp1([y next_y],[x next_x],room(i,2))  room(i,2)];
        else
            intersection = [intersection; inf 0 0];
        end
        
    end
end
%%
[min_value position] = min([intersection(:,1)]);
if min_value ~= inf
    [next_step bumping] = generate_next_step(4,2,room(position,5),k-intersection(position, 1),intersection(position, 2),intersection(position, 3),room);
    bumping = bumping + 1;
end 
