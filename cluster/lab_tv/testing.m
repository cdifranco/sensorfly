%% Initialize socket
import java.net.ServerSocket
import java.io.*
socket_port = 8964;
number_of_retries = 100; 
retry = 0;
message = '';
connection  = [];
succ_socket = 0;
try
    % wait for 1 second for client to connect server socket
    server_socket = ServerSocket(socket_port);
    server_socket.setSoTimeout(10000);
catch ME
    fprintf(2, 'socket fail to open');
    if ~isempty(server_socket)
        server_socket.close
    end
end
%% Initialization
clr = input('clear record?');
if clr == 1
    step = [];
    observed_step = [];
    error = [];
    success = [];
    start_round = 1;
else
    start_round = input('from which round:');
end
serial_port = serial('COM5', 'BaudRate', 38400, 'DataBits', 8, 'Timeout', 0.5);
%% Draw the main frame
area_number = 27;
load './loading_files/grid.mat';
Z = zeros(4);
figure;
for i =  1 : area_number
    surf(grid{i}.x, grid{i}.y, ones(4)*0, ones(4)*0);
    axis tight;
    axis equal;
    xlabel('Lab Width');
    ylabel('Lab Length');
    title('Lab Layout');
    colormap([1 1 0; 0 1 1])
    hold on;
end
axis([-inf,inf,-inf,inf,0,10]);
%% Open socket
while 1
    retry = retry + 1;
    try
        %%
        fprintf('start %d succ\n', succ_socket);
        if ((number_of_retries > 0) && (retry > number_of_retries))
            fprintf(1, 'Too many retries\n');
            break;
        end
        %%
        fprintf('try %d waiting for client to connect to this host on port : %d\n', retry, socket_port); 
        fprintf('wait for connection, get success connection %d once\n', succ_socket);
        connection  = server_socket.accept;
        %%
        succ_socket = succ_socket + 1;
        fprintf('connected\n');
        break;
    catch ME
        fprintf('connection fail\n');
    end
end
%% Set up socket IO
try
    output_stream   = connection.getOutputStream;
    input_stream = connection.getInputStream;
    d_output_stream = DataOutputStream(output_stream);
    d_input_stream = DataInputStream(input_stream);
catch ME
    fprintf('IO set up fail\n');
end
%% Open serial port
try
    fopen(serial_port);
catch ME
   error('fail to open the serial port, check connection and name'); 
end
%% Testing loop
for j = start_round:testing_round
    destArea = input('destiny area: ');
     %% Draw Dest Area
    surf(grid{destArea}.x, grid{destArea}.y, ones(4)*0, ones(4)*-1);
    axis tight;
    axis equal;
    text(22.5, 32.5, 0, 'banner');
    colormap([1 1 0; 0 1 1])
    hold on;
     %% generate the matrix
    number_of_center = size(center_new,1);
    matrix = zeros(number_of_center, number_of_center);
    for l = 1:number_of_center
        for m = 1:number_of_center
            sumIJ = sum(trans_history(count_to_id(l),:,count_to_id(m)));
            if sumIJ == trans_init_number*direction_number
                matrix(l, m) = inf;
            else
                sum_of_l_to_m = 0;
                for n = 1 : number_of_center
                    sum_of_l_to_m = sum_of_l_to_m + sum(trans_history(count_to_id(l),:,count_to_id(n)));
                end
                matrix(l, m) = sum(trans_history(count_to_id(l),:,count_to_id(m)))/sum_of_l_to_m;
                matrix(l, m) = 1/matrix(l, m);
            end;
        end;
    end;
    %% call for navigate
    [succ sigRoute] = navigate_basic(packet_id, d_input_stream, d_output_stream, serial_port, destArea, base_number, trans_history,center_sig, count_to_id, matrix, area_cluster_relation, grid);
    fprintf('you have forwarded %d steps\n',size(sigRoute,1));
     %% continue
    cont = input('still continue? (yes:1/no:0)');
    if cont == 0
        break;
    end
end
step_rate = step(step(:)~=inf)/observed_step(observed_step(:)~=inf)
ave_error = sum(error(error(:)~=inf))/size(error(error(:)~=inf),2)
%% Close port
try    
    stopasync(serial_port);
    fclose(serial_port);
    delete(serial_port);
    clear serial_port;
catch ME
    % print out warning
    error('fail to close the serial port, check connection and name'); 
end
%% Close socket
if ~isempty(server_socket)
    server_socket.close
end

if ~isempty(connection)
   connection.close
end
%% Clear 
clear cont;
clear l;
clear m;
clear j;