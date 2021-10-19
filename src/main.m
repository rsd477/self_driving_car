function main(app, on)
    app.coolant.Color = [1,1,0];
    arduino = serialport("COM3",9600);
    flush(arduino);
    while(on)
        temp = readline(arduino);
        temp = split(temp, 'x');

        if(temp(1)=='0')
            dist = str2double(temp(2))/100;
            app.distance.Text = sprintf("%1.2fm",dist);
        elseif(temp(1)=='1')
            comp = str2double(temp(2));
            if(comp==0)
                color = [0,0,0];
            elseif(comp==1)
                color = [0,1,0];
            elseif(comp==2)
                color = [1,1,0];
            elseif(comp==3)
                color = [1,0,0];
            else
                color = [1,1,1];
            end
            app.distIndicator.Color = color;
        elseif(temp(1)=='2')
            comp = str2double(temp(2));
            if(comp==0)
                color = [0,1,0];
            elseif(comp==1)
                color = [1,1,0];
            elseif(comp==2)
                color = [1,0,0];
            else
                color = [1,1,1];
            end
            app.coolant.Color = color;
        elseif(temp(1)=='3')
            temperature = str2double(temp(2));
            app.temperature.Text = sprintf("%d°c",temperature);
        elseif(temp(1)=='4')
            comp = str2double(temp(2));
            if(comp==0)
                color = [0,1,0];
            elseif(comp==1)
                color = [1,0,0];
            end
            app.TemperatureLamp.Color = color;
        elseif(temp(1)=='5')
            speed = str2double(temp(2));
            app.speed.Text = sprintf("%d%%",speed);
        elseif(temp(1)=='6')
            comp = str2double(temp(2));
            if(comp == 0)
                app.headlights.Text = "off";
            elseif(comp==1)
                app.headlights.Text = "dim";
            elseif(comp==2)
                app.headlights.Text = "high";
            end
        else
            disp(temp)
        end
    end
end

