%%Serial Communication - Matlab 2012%%
%%Souzan Mirza - 1211547 - mirzas3%%
%%Plots serial communication in 1 second intervals%%

while(true)
    state=input('Do you want to start? y/n ','s');
if state=='y'
    s=serial('COM5');
    y=[];
    fopen(s); %open serial communication port
    k=1;
    while(k==1)
        timerid=tic; %check time
        while(true)
        z=fscanf(s,'%d');
        y=[y z'];
          if(toc(timerid) >= 1.0000) %check execution time, end if longer than 1s
             break;
          end
        end
        y=(5/2^10).*y; %convert the codeword to a voltage=(codeword/2^10)*5V
        plot(y);
        xlabel('Samples per second');
        ylabel('Voltage');
        axis([0 inf 0 5]);
        state=input('Do you want to start?','s');
        if(state=='y')
            y=[];
        end
        if(state=='n')
           k=0;
        end 
    end
end
if state=='n'
    fclose(s); %close port
    break;
end
end