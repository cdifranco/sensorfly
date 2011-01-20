import java.net.ServerSocket
import java.io.*

port = 8964;
number_of_retries = 100; 
retry = 0;
message = 'aaa';
connection  = [];
succ = 0;
%%
try
    % wait for 1 second for client to connect server socket
    server_socket = ServerSocket(port);
    server_socket.setSoTimeout(10000);
catch ME
    error('socket fail to open');
end
%%
while 1
    retry = retry + 1;
    try
        fprintf('start %d succ\n', succ);
        if ((number_of_retries > 0) && (retry > number_of_retries))
            fprintf(1, 'Too many retries\n');
            break;
        end

        fprintf(2, ['Try %d waiting for client to connect to this ' ...
                    'host on port : %d\n'], retry, port);
    
        fprintf('wait for connection, get success connection %d once\n', succ);
        connection  = server_socket.accept;

        succ = succ + 1;
        fprintf('Connected\n');
        break;
    catch
        fprintf('Connection fail\n');
        %if ~isempty(server_socket)
        %    server_socket.close
        %end

        %if ~isempty(connection)
        %    connection.close
        %end

        % pause before retrying
        pause(1);
    end
end
%%
while 1
    cont = input('continue?');
    if cont == 0
        break;
    end
    
    try
        output_stream   = connection.getOutputStream;
        input_stream = connection.getInputStream;
        d_output_stream = DataOutputStream(output_stream);
        d_input_stream = DataInputStream(input_stream);

        
        fprintf('start to receive\n');
        receive_msg = socket_receive(d_input_stream, 3)
        fprintf('end of receive\n');
        
        % output the data over the DataOutputStream
        % Convert to stream of bytes
        message = receive_msg;
        fprintf('Writing %d bytes\n', length(message));
        socket_send(d_output_stream, message);
    catch
        fprintf('IO fail\n');
        %if ~isempty(server_socket)
        %    server_socket.close
        %end

        %if ~isempty(connection)
        %    connection.close
        %end

        % pause before retrying
        pause(1);
    end
end

%% Clean up
if ~isempty(server_socket)
    server_socket.close
end

if ~isempty(connection)
   connection.close
end
