# Small Soft pack
Made for God Glory.  
Hail Supreame God one in great trinity.  
Prise Saint Maria, mother of God.  

A pack of my little unix programs.  

You can either read this file as plain text  
( and still understand 100% of what is this read me about ).  
Or you can use your webbrowser to read it:  
>`firefox readme.html`  

## INSTALLATION:

### 1) downloading
Before we install, we need to suck this up:  
>`git clone 'https://github.com/ignotaC/smallsoft'`  

### 2) compilation
Run make in makefile:  
>`make`  

Make before compilation will upload  
required submodules - ignotalib.  
Compiled programs will go to the newly created  
bin directory inside smallsoft.  

### 3) installation ( binary managment )
You do it by your self, You might no want to  
use some of the software this pack provides.  
Simply copy what ever you want to one of the  
preffered PATH directory which you can see  
after running env command. Since I add  
~/bin to my PATH with .xsession.  
I simply take all by doing:  
>`cp bin/* ~/bin`  

### 4) clean up
This simply removes stuff make created  
inside the directory of your makefile.  
>'make clear'

## Program list:

1.  **coin**  
  >  

    Program lets you find an answer to a 0/1 problem.  
    Program does not take any arguments.  
    Dependencies: none  
    Example of using it:  
  >`coin` 

2.  **getproxies** `[ string ]`  
  >  

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
    your IP if for example user-ganet will not fit their  
    expectations.  

3. **novena**
  >  

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

4. **workout**
  >  

    Program generates workout entries for each day. Entries  
    are stored inside .workout file in enviroment home  
    directory. Program does not take any arguments.  
    The file is generated automatically when program is run.  
    For current day it shall output:  

    * exercise type,  
    * weight level - how much load you will lift   
    * series count per exercise  
    * Hardness - how many times repeate per one series  
    * Count of exercise type  

    Dependencies: none  
    Example of using it::  
  >`workout`  
    

5. **logtime**
  >  

    Program logs time per each line  that appears at stdin.  
    Output format:  
  >`<seconds>.<miliseconds><space><passed line>`  

    Dependencies: none  
    Example of using it:  
  >`program | logtime`  

    You should rememer that | is buffered and will not  
    pass everything immediatly always. So the real time  
    will can be different than the loged one.  

6. **recev**
  >

    Program creates a record of pressed mouse buttons,  
    keyboard keys and mouse moves. Program at start  
    will ask you are you ready. After that you will  
    need to decide what key will start and stop recording.  
    You can chose only single keys. Combinations do not  
    work. You need to type the key so for example  
    it could be 'F8' or h. After the key is picked,  
    you must provide a filenmae, where the event data  
    gets recorded. File will be placed in working  
    directory. Events that do not matter and repeated  
    mousemoves are removed to shrint at least a bit the  
    file where events get saved.  Since it can grow quite  
    big. This program does not take any arguments.  
    Aditionaly **xenv** program can help with finding key name.  
    Dependencies:  **xmodmap**, **xinput**  
    Example of using it:  
  >`recev`

7. **playev** `[ file name ] [ number ] [ seconds ] [ seconds ]`
  >

    Program playes events from file created by recev program.  
    Program needs four arguments:  

    * file name - it is the file created by recev.  
    * number of times event gets repeated, 0 causes  
    to be repeated infinite times.  
    * seconds to wait before playing events loop  
    * seconds to wait between instance of recorded events.  
    
    Dependencies: **xmodmap**  
    Example of using it:  
  >`playev recorded_file 100 10 1`

    This will play event recorded in passed file name  
    100 times. Program will start playing events after  
    10 seconds and each time all events in recorded  
    file are played, it will wait 1 second and replay the  
    events. Untill reached the expected repeated number.  

8. **gethostipv** `[ -46 ] [ domain ]`
  >

    Program uses system resolver tools to get the ip  
    of the domain. Program expects at least the domain name  
    argument. Order of arguments does not matter. But at  
    maximum there can be only two arguments. So the options  
    must not be separated like this: `-4 -6`  
    Instead it should look like this: `-46`  
    So we have four possibilities of passing options:  
  >`-4 or -6 or -46 or -64`  

    Options:  

    `-4`  
    Look only for IPv4 address  

    `-6`  
    Look only for  IPv6 address  

    Address shall only be outputted to stdout if it is possible  
    to connect to it. Mapping ipv4 to ipv6 is not checked.  
    So if the host has ipv6 and ipv4  but we can't connect to ipv6  
    and instead we need to use ipv4. When flag `-46` was passed,  
    ipv4 will be shown.  But if we passed option`-6`.  
    Nothing shall be outputted. If host has more addresses,  
    and we can connect to each of them.  
    All will be outputted. Passsing `-46` means - check first for  
    ipv4, than for ipv6. If `-64` vice versa.  
    If we pass domain name without options. Both ipv4 and ipv6  
    are checked, but the order is undefined.
    Dependencies: none  
    Example of using it:  
  >`gethostipv -64 www.dom.com`  

    In which we ask to output first ipv6 possible addresses  
    that we could connect to and than ipv4. For the  
    www.dom.com domain.  

9. **repeatstr** `[ number ] [ string ]`
  >

    Program repeats passed phrase specified number of times.  
    There are always expected two arguments.  
    First one is number of times string will be repeated.  
    Second is the string it self that program will repeat.  
    Supported character sequances:  

    * `\a` = bell  
    * `\b` = backspace  
    * `\f` = form feed  
    * `\n` = new line  
    * `\r` = carrige return  
    * `\t` = tab  
    * `\v` = vertical tab  
    * `\\` = backslash  

    Dependencies: none  
    Example of using it:  
  >`repeatstr 100 'this gets repeated 100 times'`

10. **lext** `[ option ] [ string ]`  
  >

    Program extracts strings form stdin. Don't pass any  
    arguments to it and help will be outputed.  
    For proper use, this program expects arguments.  
    There are three options to chose from. 
    Options:  

    `-p _string_`  
    Pass data untill _string_ is located.  

    `-c _string1_ _string2_`  
    Output data between _string1_ and _string2_  
    Which both are on same line.  

    `-l`  
    Loop last command.  

    After EOF is met - program ends.  
    Dependencies: none  
    Example of using it:  
  >`cat datafile | strext -p 'dev' -c 'name="' '"' -l`  

    This will read datafile from stdin untill 'dev' is met.  
    After that program will look for data which lies between  
    'name="' and '"' where both strings will be on the same line.  
    Finally loop the last command, look for dara between  
    specified strings untill EOF met on stdin.

11. **getlimits**
  >

    Program prints system soft and hard resource consumption.  
    Basic are:  

    * Maximum core file size  
    * Maximum CPU time used by each process  
    * Maximum size of data segment including allocations  
    * Maximum size of file that can be created  
    * Maximum number of file descriptors process can open  
    * Maximum size of process stack  
    * data segment etc.  

    Few other system dependant also are printed for linux and  
    OpenBSD.  
    Dependencies: none  
    Example of using it:  
  >`getlimits`  

12. **fstrswp** `[ string ] [ string ]`
  >

    Program will read line and swap any found  string from first argument,  
    to string in second argument. I use it for source fixing.  
    Each line gets allocated to heap. After swapping is done  
    output is sent to stdout anf line gets freed.  
    Dependencies: none  
    Example of using it:  
  >`find src -type f -iname '*.c' -exec sh -c \`  
  >`'cat {} | fstrswp '\''malloc.h'\'' '\''stdlib.h'\'\`  
  >`' | tmpfile; mv tmpfile {}' \;`  

    This fixes gnuism crap left in code by some  
    unexpiranced coder. Next run would scan `*.h`.  

13. **parsekdump**
  >

    This program is openbsd ktrace output parser.  
    It shall count systemcalls and create a summary  
    of them. It outputs data per PID/TID and on the end  
    summary of all treads and forks.  
    Each basic data line will contain:  

    * System call name 
    * Count of times it was called in the process 
    * Procent of system call summary compared to all system calls time
    * Procent of system call summary to record time

    Dependencies: openbsd operating system  
    Example of using it:
  >`ktrace -i program`  
  >`parsekdump`  

    ktrace -i option is for tracking forks and threads. I advice to track all  
    else the output summary might be missleading.  

14. **ualist**
  >

    Program removed, it makes more sens to provide a list of useragents from  
    a server. Which I will soon do.

15. **randstr** `[ option ] [ number ]`
  >

    Program generates random string. It uses pseudorandom generator,  
    Deterministic C rand().  
    Without any options any bytes can appear.  So it can even generate nul.  
    To force more usefull output we have options which we can mix:  

    `-u`  
    Print out only upper case output.  

    `-l`  
    Print out only lower case output.  

    `-d`  
    Print out only digits  

    There is no expected order of arguments.  
    Dependencies: none  
    Example of using it:  
  >`randstr -uld 50`  

    In this example we generate 50 characers where each of them  
    must be a digit or an upper/lower case alphabet number.  
    Second example above does the same thing, as we can see,  
    options can be together and can repeate.  
  >`randstr -d 50 -uuuud -udl`  

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

