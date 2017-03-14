function client(port)
%   provides a menu for accessing PIC32 motor control functions
%
%   client(port)
%
%   Input Arguments:
%       port - the name of the com port.  This should be the same as what
%               you use in screen or putty in quotes ' '
%
%   Example:
%       client('/dev/ttyUSB0') (Linux/Mac)
%       client('COM3') (PC)
%
%   For convenience, you may want to change this so that the port is hardcoded.
   
% Opening COM connection
if ~isempty(instrfind)
    fclose(instrfind);
    delete(instrfind);
end

fprintf('Opening port %s....\n',port);

% settings for opening the serial port. baud rate 230400, hardware flow control
% wait up to 120 seconds for data before timing out
mySerial = serial(port, 'BaudRate', 230400, 'FlowControl', 'hardware','Timeout',5); 
% opens serial connection
fopen(mySerial);
% closes serial port when function exits
clean = onCleanup(@()fclose(mySerial));                                 

has_quit = false;
% menu loop
while ~has_quit
    fprintf('PIC32 MOTOR DRIVER INTERFACE\n\n');
    % display the menu options; this list will grow
    fprintf('     a: Read current sensor(ADC counts)   b: Read Current Sensor(mA)\n');
    fprintf('     c: Read encoder(counts)              d: Read encoder(deg)\n');
    fprintf('     e: Reset encoder                     f: Set PWM(-100 to 100)\n');
    fprintf('     g: Set current gains                 h: Get current gains\n');
    fprintf('     i: Set position gains                j: Get position gains\n');
    fprintf('     k: Test current control              l: Go to angle(deg) \n');
    fprintf('     m: Load step trajectory              n: Load cubic trajectory\n');
    fprintf('     o: Execute trajectory                p: Unpower the motor\n');
    fprintf('     q: Quit client                       r: Reset Encoder \n');
    % read the user's choice
    selection = input('\nENTER COMMAND: ', 's');
     
    % send the command to the PIC32
    fprintf(mySerial,'%c\n',selection);
    
    % take the appropriate action
    switch selection
        case 'a'
            ADCcount = fscanf(mySerial,'%d');
            fprintf('Current sensor in ADC counts: %d counts\n\n',ADCcount);
        case 'b'                         
            current = fscanf(mySerial,'%f');   
            fprintf('Current sensor in mA: %.2f degrees\n\n',current);
        case 'c'                      
            n = fscanf(mySerial,'%d'); 
            fprintf('The motor angle is: %d counts\n\n',n);  
        case 'd'                        
            n = fscanf(mySerial,'%f');  
            fprintf('The motor angle is: %.2f degrees\n\n',n); 
        case 'e'
            fprintf('Motor position is now at zero position.\n\n');
        case 'f'
            DC = input('Enter a Duty Cycle (-100 to 100): ');
            fprintf(mySerial,'%f\n',DC);
        case 'g'
            Kp = input('Enter Kp current gains: ');
            fprintf(mySerial,'%f\n',Kp);
            Ki = input('Enter Ki current gains: ');
            fprintf(mySerial,'%f\n',Ki);            
        case 'h'
            KP = fscanf(mySerial,'%f');
            KI = fscanf(mySerial,'%f');
            fprintf('Kp: %.2f, Ki: %.2f\n\n',KP, KI);
        case 'i'
            Kp = input('Enter Kp current gains: ');
            fprintf(mySerial,'%f\n',Kp);
            Ki = input('Enter Ki current gains: ');
            fprintf(mySerial,'%f\n',Ki);            
            Kd = input('Enter Kd current gains: ');
            fprintf(mySerial,'%f\n',Kd);            
        case 'j'
            KP = fscanf(mySerial,'%f');
            KI = fscanf(mySerial,'%f');
            KD = fscanf(mySerial,'%f');
            fprintf('Kp: %.2f, Ki: %.2f, Kd: %.2f\n\n',KP, KI, KD);
        case 'k'
            fprintf('Testing current gains\n\n');
            read_plot_matrix(mySerial);
        case 'l'
            angle = input('Enter a desired angle position: ');
            fprintf(mySerial,'%f\n',angle);
        case 'p'
            fprintf('Powering off motor...');
        case 'q'
            has_quit = true;             % exit client
        case 'r'                      
            s = fscanf(mySerial,'%s');
            fprintf('Current mode is ');  
            fprintf(s);  
            fprintf('\n');
        otherwise
            fprintf('Invalid Selection %c\n', selection);
    end
end

end
