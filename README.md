# Small Soft pack
Made for God Glory.  
Hail Supreame God one in great trinity.  
Prise Saint Maria, mother of God.  

A pack of my little unix programs.  

You can either read this file as plain text  
( and still understand 100% of what is this read me about ).  
Or you can use your browser to read using webbrowser:  
>`firefox readme.html`  

## INSTALLATION:

### 1) downloading
Before we install, we need to suck this up:  
>`git clone 'https://github.com/ignotaC/smallsoft'`  

Now enter the newly created directory:  
>`cd smallsoft`  

Download submodules:  
>`sh code_update.sh`  

### 2) compilation
Run make in makefile:  
>`make`  

Compiled programs will go to the newly created  
bin directory inside smallsoft.  

### 3) installastion ( binary managment )
You do it by your self, You might no want to  
use some of the software this pack provides.  
Simply copy what ever you want to one of the  
preffered PATH directory which you can see  
after running env command. Since I add  
~/bin to my PATH with .xsession.  
I simply take all by doing:  
>`cp bin/* ~/bin`  

### 4) cleanup
This simply removes stuff make created  
inside the directory of your makefile.  
>'make clear'

## Program list:

1. **coin**

    Program lets you find an answer to a 0/1 problem.  
    Program does not take any arguments.  
    Dependencies: none  
    Example of using it:  
  >`coin` 

2.  **getproxies** `[ word ]`  
  Program filters few webpages to suck up proxies,  
  also checks  if they work properly. Data is saved  
  to a file named  "proxylist" inside working directory.  
  Slow proxies that do not respond quick enought and  
  resoults that repeated are removed.  
  The file will contain each uploaded proxy data per line.  
  Lines will be formated like this:  
  >`<IP> <port> <country> <anonimyty type> <http or https support>`

    Program takes arguments. If line contains any  
    of the passed words, the line will be removed.  
  The search is case insensetive. So passing  
  'http' or 'HTTP' will remove any line that contains 'http'  
  Dependencies: **wget**  
  Example of using it:  
  >`getproxies china`  

    This will create the 'proxylist' file with resoults but  
  without lines containing word 'china'. Upper or  
  lower cases won't matter. So line contaning 'chINA'  
  will also be removed. China proxies most times are garbage,  
  so you want them removed. Other type of use would be  
  getting rid 'transparet' proxies.  Altho you should never  
  trust anonimity type of proxy. Some servers will pass  
  your IP if for example user-ganet wil

3. **novena**
  Program will save file in home directory and track what  
  current day of novena we have. Program does not take any  
  arguments. It will either tell you the current novena  
  day (novena lasts 9 days always). Or it will ask you  
  for new novena name. Program creates in user home  
  directory taken from enviroment list .novena file. Where  
  it keeps data. If 9 days pass program will inform you  
  the novena has ended.  
  Dependencies: none  
  Example of using it:  
  >`novena`  

4. **workout** - Program generates workout entries
             for each day. Keep training!
             Creates .workout file in env HOME

5. **logtime** - Logs time from stdin per line and
             attaches it on stdout. format is
             <seconds>.<miliseconds><space>
             Example how to use:
             echo 'black jews' | logtime
             Output bill be similar to this:
             18273553.746236 black jews

6. **recev** - Record events from mouse and keyboard.
           It needs to work xinput, and xmodmap.
           Also xev might be useful. Record before
           processing (all happen in one run) 
           might make a big file. You tell the
           program key you choose to start/stop
           recording and file where he saves
           the record. To play recorded events
           use playev program from this list.

7. **playev** - Program playes events saved in file
            created by recev program. In general
            you just use it like this:
            playev recev_file x y z
            Where ofc first arg is file name of
            event file, *x* is how many times
            you want to repeat playing event
            record, 0 means repeat forever.
            The *y* is time before starting to
            play event loop and *z* is time
            between every event record replaying.

8. **gethostipv** - use system resolver tools to get the ip of the domain.
            Ip version we can't connect to ( because we only have ipv4
            and server is on only ipv6 ) is not returned.
            how to use: gethostip [-command] [domain]
            Order does not matter but there can be only one command string
            In the end you can pass these kind of commands:
            none - just return what ever ip will work, if none - nothign is returned
            -6  - check only for ipv6 if no support nothing will be returned
            -4  - check only for ipv4 if no support nothing will be returned
            -46 - check first for support for ipv4 than for ipv6
            -64 - check first for support for ipv6 than for ipv4
            all other commands will puke an error.

9. **repeatstr** - Extreamly basic program to repeate on stdout phrase.
            example: repeatestr 100 'stuff happened'
            supported character sequances: \\ \r \n \t \v \f \b \a

10. **strext** - In general it's a string extractor form a file.
             It has three basic commands. Pass to pass file
             untill a string is met. Cut that find strings
             in line and outputs on stdout string between them.
             last command is loop. It will loop for ever last
             command. Only has sens to use after cut command.
             Ofc loop will end with eof at stdin. Pass nothing
             and help will be printed.

11. **getlimits** - Program prints current soft and hard limits
             for user. CPU, stack, data segment etc.
             See for your self simply run it, no arguments.

12. **fstrswp** - Swap one string in stream to other. 
              Perfect for some source fixing.
              For example when we need to swap malloc.h to stdlib.h.
              this would look like this:
              find src -type f -iname '*.c' -exec sh -c \
                   'cat {} | fstrswp '\''malloc.h'\'' '\''stdlib.h'\'' ' \
                   '| tmpfile; mv tmpfile {}' \;
              Could be without '\'' but what if we have spaces or worse?

13. **parsekdump** - This is wrote for openbsd ktrace output. Not sure how it
                 would work on other oses. Anyway you ktrace your program
                 eventualy to keep track of forks ktrace -i program.
                 Than run parsekdump and it will try to turn kdump output
                 into lists of system calls and their percent time.
                 Basic PID/TID output makes one line per system call.
                 line containes:
                 1. System call
                 2. How many times system call was called and returned
                 3. Precent of all system calls times it took
                 4. Precent it took compared to recording time.
                 last one takes recording time as first and last appearance
                 of CALL or RET.

14. **ualist** - Program will output little list of user agents to stdout.
             You use it like this: ualist proxyip:proxyport > my_list.
             It uses torsocks wget to download specific webpage
             Torsocks is only for not getting real IP banned.
             The page owner bans a lot so it's better to hide and
             you should not trust proxies.
             Anyway use getproxies and pass one of lister by it proxy ip port
             or use your own.
             CRUCIAL: use https proxies.
             They will surely ban it after you use it.
             So open free proxies are best thing. Since they will die out
             in few days anyway.
             Program sleeps a bit and is slow. If you stop getting output
             change the proxy. The owner of page we get UA is a bit dramatic.
             So you need to be stealthy.

15. **randstr** - Program generates using pseudorandom generator ( C rand() )
              Randomn string. You must pass number of bytes to generate.
              Also you can force to output onlu uppercase letters, lowercase
              and digits. With commands -u -l -d.
              Example: randstr -u 124 -ld. This will generate 124 bytes
              that in asci code appear as lowercase uppercase or digits.

16. **logdata** - Program opens unix socket and prints all data sent to it.
              Example of using it:
              We start it: logdata unix_socket_name_we_pass > savehere
              Now we need to send some data using netcat.
              Openbsd netcat can send to unix socket. 
              Many linux distros have in their repository: openbsd-netcat
              or something similar. So we can with nc:
              echo "test" | nc -UN socket_name
              -N option is a must. Socket must sent FIN else the connection stales.
              SIGINT logdata for gentle close when not anymore needed!

17. **runprog** - Program opens unix socket and runs program
              or script we pass at it start up. It runs the command when
              there is opened connection. It ignores sent data.  You can either
              not even wait for the program to finish running the command
              or wait.
              I decided that there is no sens for inet socket type feature.
              Unix socket is better fit for IPC without outside interferation.
              Example: ./runprog '/tmp/sun' './runthisscript';
              Than ask it to run it but don't wait for it to finish: nc -Uz '/tmp/sun'
              Or ask it to run it and wait till it finish: nc -Ud '/tmp/sun'
              Runprog closes gently when recives sigint. Additional one more argument
              is for groups. You can do:
              ./runprog '/tmp/sun' './runthisscript' 'go'
              last one go stand for group and other - write permissions set on
              unix socket. So if you need root to execute something simple user
              can call it via this socket to do the job.

18. **httpreq_addcrlf** - Program adds after new line of each entry CRLF creating
              proper http request. On the end it makes passes one more CRLF.
              Empty lines are being ignored. Example of usage, we have file httpreq:
              
              GET /babylon/tower HTTP/1.0
              Host: gagle.shit
              
              We use it this way:
              cat httpreq | httpreq_addcrlf | netcat gagle.shit 80

19. **flinemem** - program loads stdin line to memory and searches if passed arguments
	      exist in line. Line is printed to stdout only if all strings exist in line.
              No need for regular expression when you simply need to find static patterns.
              Example fo using:  cat file | flinemem 'pattern1' 'pattern2' 'pattern3' 

20. **urlcode** - at the moment we have 3 options, encode (-e), decode (-d)  and
              'make post body' (-p). How to use:
              echo 'string to @#$ encode" | urlcode -e | urlcode -d
              This will encode irl string and than decode it. Now the post part is a bit tricky.
              First argument after options gets encoded and second argument does not.
              Than third gets encoded and fourth does not. And so on.
              We would use it in script like this:

              postbody=$( encode -p 'login' '=' 'someone' '&' \
                                    'pass' '=' '7ygft67$%^&*some' '&' \
                                    'hidden_token' '=' '12fjwhr@#$Rwef345r3tGF#$$' )
              # at this moment we find out the lengh of body and pass it for contetn lengh 
              # after forming the http header we simply send this stuff like this using 
              # former tools from this soft pack ( httpreq_addcrlf ):
              { cat httpreq | httpreq_addcrlf | cat; printf "%s" "$postbody"; } | \
              nc -c -X 5 -x localhost:9050 www 443

              # Hope this is clear. The last line shows nc with passing traffic via tor
              # for server with https.

21. **givetask** - Simple program, opens unix socket from first argument. And outputs each line
               from stdin per connection. Then shutdowns and closes each socket. Example:
               cat file | givetask /tmp/some; nc -U /tmp/some 

22. **miodpitny** - program tells you MEAD proportions. 

23. **irclog** - program to log irc. It's old code. Not too proud of it but I needed to log
             some stuff recently and it worked. So I am adding it here for now.
             Will try to reread it in future for detecting some bugs and other crap.
             How to use: create a file in directory where you will start the irclog program
             The only accepted file name is: log_serv_data
             Else error.
             Contents example:
             Sirc.liberapriviligesqwertycrap.chat 6667
             MNICK dicky 
             MUSER dicky ** :Philip_DICK
             C##channel_1
             C#devchannel_2
             C##channel_3
             
             Now, S is for server where you pass domain port
             M is for first message you provide at start up, irc proto
             C is for channel you want to join
             There is also I for identication string. I rememebr ages ago it worked but now?
             This will have sooner or later update. But since it works than I am putting it up here.
             Expect broken behavior. It was intended to be able log multiply servers. So next S in 
             config file should merit new network.
             Each channel gets it's own file where stuff gets logged in the directory where you 
             started the program. <networkname><channame> <- something like this
24. **faddlines** - Program merges files. pritnf on stdout added lines like this:
             line 1 file 1 + space + line 1 file 2\n
             If one file reached eof it gets ignored
             after all files reach EOF - program finishes.
             Where I use it:

             Let us imagine we have y(t) and x(t);
             We want to plot it in gnuplot;
             We could also have already data files out there.
             Taken from some sensor.

             echo 'for(i=0.1; i < 100; i+=0.1) { i-5 }' | bc > file1
             echo 'for(i=0.1; i < 100; i+=0.1) { i+5 }' | bc > file2
             faddlines file1 file2 > ans;
             ## After that I can plot x,y points with gnuplot to see how it looks.
             gnuplot
             >> p ans

             This program can take multiply files. 

25. **cutfile** - program stdouts lines after meeting in line some expression.
              For help run cutfile without any arguments

26. **htmlentities** - decode or encode html entities, only some are supported.
                   suported: '<' '>' ' ' '\'' '"' '&' . Rets is treated as errror.
                   honestly I dislike utf8. Let it rot. 
                   How to use -> invoke without any arguments. Still little example:
                   cat urllinksfromhtmlpage | htmlentities -d > proper_url_links

27. **httphb_split** - Splits http server answers to http header or the rest ( body )
                   For help simply run it without any arguments.
                   Example: http_serber_answer | httphb_split -h > httpheader_file
28. **fixedxarg** - Program closly performs same task as xargs, BUT PROPERLY.
                Each new line separates new argument. Empty lines creates ''
                Which normal xarg fails to catch. No options. Example:
                echo  "a\nb\nc\nd\n" | fixedxarg echo
29. **listwords** - 

