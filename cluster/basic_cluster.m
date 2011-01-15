center = []; %cluster_id, contain_reading_number, real_x, real_y,sig1,sig2,sig3...sigN
sig_count = 1;
trans_history = [];     
bel = [];
bel(1:size(center,1)) = 1/size(center,1);
serial_port = serial(port,'BaudRate',38400,'DataBits',8,'Timeout', 0.5);
% open the serial port
try
    fopen(serial_port);
catch ME
   % print out warning
   error('fail to open the serial port, check connection and name'); 
end
reading = zeros(main_loop_count, 4+base_number);
% main loop
for mainloop = 1 : main_loop_count
    fprintf('round %d\n',mainloop);
    node_id = 12;
    %initiate the believe vector
    reading(sig_count,1) = 0;
    fprintf('get readings\n');
    %get_dir = input('ready to get direction? (1:yes; 0:no)');
    %if get_dir == 0
    %    break;
    %end
    tic;
    reading(sig_count, 3) = get_area();%area id 
    reading(sig_count, 4) = 0; %researved element in the structure
    [reading(sig_count, 5:4+base_number) packet_id] = get_sig_from_port(packet_id, serial_port, base_number);
    [reading(sig_count,2) packet_id] = get_dir_from_port(packet_id, node_id, serial_port);
    toc;
    % initialize the bel_bar
    bel_bar = zeros(1,size(center,1));
    for j = 1:size(center,1)
        total_count = sum(trans_history(j, reading(sig_count-1,2), :));
        for k = 1:size(center,1)
            if total_count == 0
                trans_p = 0;
            else
                trans_p = trans_history(j, reading(sig_count-1,2), k)/total_count;
            end
            bel_bar(k) = bel_bar(k) + trans_p * bel(j);
        end
    end
    % get the distance reading and the new bel
    for j = 1:size(center,1) %check all the centers
        edist = sum((reading(sig_count,5:end)-center(j,5:end)).^2).^.5;
        p = possibility(edist,distribution_table{base_number});
        bel(j) = p * bel_bar(j);
        if bel(j) > bel_threshold && (reading(sig_count,1) == 0 || bel(j) > bel(reading(sig_count,1)))
            reading(sig_count,1) = j;
        end         
    end
    if reading(sig_count,1) ~= 0
        center(reading(sig_count,1),2) = center(reading(sig_count,1),2) + 1;
    else
        % add the new center to the count
        temp = [];
        temp = [temp size(center,1)+1];
        temp = [temp 1];
        temp = [temp reading(sig_count,3:end)];
        center = [center; temp];
        reading(sig_count,1) = size(center,1);
        bel(size(center,1)) = 1;
        %update the trans_history
        trans_history(1:size(center, 1), 1:direction_number, size(center, 1)) = trans_init_number;
        trans_history(size(center, 1), 1:direction_number, 1:size(center, 1)) = trans_init_number;
    end
    if mainloop ~= 1
        trans_history(reading(sig_count-1,1), reading(sig_count-1,2), reading(sig_count,1))=trans_history(reading(sig_count-1,1), reading(sig_count-1,2), reading(sig_count,1))+1;
    end
    %normalize the bel
    bel_total =sum(bel(:));
    bel = bel / bel_total;
    sig_count = sig_count + 1;
    save '1_15_morning.mat';
    pause(1);
end
% close port
try
    stopasync(serial_port);
    fclose(serial_port);
    delete(serial_port);
    clear serial_port;
catch ME
    % print out warning
   error('fail to close the serial port, check connection and name'); 
end
%filter the centers
cn = 1/size(center,1)*center_filter*main_loop_count;
center_new = [];
count_to_id = [];
for cc = 1 : size(center,1)
   if center(cc,2) > cn 
      center_new = [center_new; center(cc,:)];
      count_to_id = [count_to_id, center(cc, 1)]
   else
      rc = 1;
      while rc <= size(reading, 1)
      	if reading(rc, 1) == center(cc, 1)
            reading(rc, :) = [];
        else
            rc = rc + 1;
        end
      end
   end
end
center_sig = center_new(:,5:end);
save '1_15_morning.mat';