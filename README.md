# Small Soft pack
Made for God Glory.  
Hail Supreame God one in Great Trinity.  
Prise Saint Maria, mother of God.  

irc: ##ignota irc.libera.chat

A pack of my little unix programs.  
I do not mention dependencies like C compiler and posix system.  
None means you have the basic set.  
Also for automated compilation you need make.  

I wrote this code for my personal use.  
Shell hacks for life.

You can either read this file as plain text  
( and still understand 100% of what is this "read me" about ).  
Or you can use your webbrowser to read it:  
>`firefox readme.html`  

## INSTALLATION:

### 1) downloading
Before we install, we need to suck this up:  
>`git clone 'https://github.com/ignotaC/smallsoft'`  

### 2) compilation
Run make:  
>`make`  

Make before compilation will upload  
required submodules - ignotalib.  
Compiled programs will go to the newly created  
bin directory inside smallsoft.  

### 3) installation ( binary managment )
You do it by your self, You might not want to  
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

2.  **getproxies** `[ pattern ]`  
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
    can be different than the loged one.  

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
    argument. Argument order matters, first options,  
    last argument is domain.  
    It can look like this:  
  >`-4 or -46 or -6 -4 `  

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

9. **repeatstr** `[ number ] [ pattern ]`
  >

    Program repeats passed phrase specified number of times.  
    There are always expected two arguments.  
    First one is number of times string will be repeated.  
    Second is the string it self that program will repeat.  
    Supported character sequances:  

    * `\a` = bell  
    * `\b` = backspace  
    * `\f` = form feed  
    * `\r` = carrige return  
    * `\n` = new line  
    * `\i` = This is very special, you can pass it in pattern  
    only one time, it will increment from zero to  number - 1
    * `\t` = tab  
    * `\v` = vertical tab  
    * `\\` = backslash  

    Dependencies: none  
    Example of using it:  
  >`repeatstr 100 'this gets repeated 100 times'`  
  >` `  
  >`repeatestr 3 'Increment number: \i'`  

    Last example output wil look like this:  
  >`Increment number: 0`  
  >`Increment number: 1`  
  >`Increment number: 2`  

10. **lext** `[ -clp ] [ pattern1 ] [ pattern2 ]`  
  >

    Program extracts strings form stdin. Don't pass any  
    arguments to it and help will be outputed.  
    For proper use, this program expects arguments.  
    There are three options to chose from. 
    Options:  

    `-c _string1_ _string2_`  
    Output data between _string1_ and _string2_  
    Which both are on same line.  

    `-l`  
    Loop last command.  

    `-p _string_`  
    Pass data untill _string_ is located.  


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

12. **fstrswp** `[ pattern1 ] [ pattern2 ]`
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

    This program is openbsd ktrace/kdump output parser.  
    It shall count system calls and create a summary  
    of them. It outputs data per PID/TID and on start  
    summary of all treads and forks.  
    Summary will contain:

    * System call name
    * Count of times it was called in all forks and threads

    Each basic data line for PID/TID will contain:  

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

14. **rmvmb** [-f]
  >

    Program removes multibyte characters from input.  
    Normal one byte characters are redirected to stdout.  
    Rarely you might encounter some illegal characters.  
    Usually some old unknown encodings.  
    In that case use -f option. It will pass unknown characters.  
    Instead of throwing out error.  
    Dependencies: none  
    Example of using it:
  >`echo "$string_with_multibytechars" | rmvmb`  
  >`echo "$string_with_brokenmultichars" | rmvmb -f`  

15. **randstr** `[ -uld ] [ number ]`
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

16. **sndlog_data**  `[ -options ] [ socket info ]`
  >

    Program opens unix or internet socket and either sends  
    data or logs it. It uses TCP protocol, by default picks IPv6 if  
    both intrefaces ( IPv6 and IPv4 ) are present.  
    You must always pass an option string and socket information.  
    Existing options:  

    `-l`  
    Log data, everything that is recieved form  
    connected clients is being redirected on stdout.
    Expect FIN at the end of data.  
    It is an error to pass this option with `-s` option.  

    `-s`  
    Send data, send one line from stdin to every client  
    that connects, than pass FIN and close connection.  
    It is an error to pass this option with `-l` option.  

    `-u`  
    Use unix socket for server. Second argument - socket  
    information, must contain file name unix socket  
    will use. It is an error to pass this option with `-i`  

    `-i`  
    Use internet socket. Second argument - socket infromation  
    is port number we want server to use. It is an error  
    to pass this option together with  `-u`.
    
    All data send to the socket will be redirected to stdout.  
    Gentle way of telling server to finish running is sending SIGINT signal to it.  
    dependencies: none  
    Example of using it:  
  >`sndlog_data -li 1500`  

    This will create internet socket under 1500 port number  
    and output incoming data to stdout. We could send it data  
    using `nc` like this: `echo 'weeb' | nc -N 'localhost' '1500'`  
  >`sndlog_data -su /tmp/sun_name`  

    This will create unix socket file just as socket information says
    under '/tmp/sun_name'. Server will send every connection one line  
    from stdin. We could log data using `nc` since we like it so much.  
    just like this: `nc -U '/tmp/sun_name'`  

17. **httpreq_addcrlf**
  >

    Program adds after new line of each entry CRLF creating  
    proper http request. After reaching EOF  one more CRLF  
    is being sent that indicates end of HTTP head.  
    Empty lines are being ignored.  
    Dependencies: none  
    Example of using it:  
  >`cat httpreq | httpreq_addcrlf | nc hate-google.com 80`  

    Where httpreq file would look like this:  
  >`GET /babylon/tower HTTP/1.0`  
  >`Host: hate-google.com`  

18. **flinemem** `[ phrease1 ] [ phrease2 ] ...`
  >

    This simple program loads from stdin line to memory and searches  
    if passed arguments exist in line. Line is printed to stdout only  
    if all strings exist in line. First of all it's much faster then  
    grep -E ( like 8 times ) since regular expressions are not used,  
    and also, the egrep ( depracated name )  syntax for like  
    four arguments and more is nonsens and shit.  
    Command gets insanely long because someone could not make it simply  
    "phra1&phra2" or what ever.  This program has none of such issues.  
    Dependencies: none  
    Example fo using it:  
  >`cat file | flinemem 'pattern1' 'pattern2' 'pattern3'`

19. **urlcode** `[ -options ] [ data1 ] [ data2 ] [ ... ]`
  >

    Encode or decode urlcode. + <=> space is not supported  
    It's a browser extension. We have three types of options  
    and we can choose only one.  

    `-d`  
    Decode passed data from stdin and send it to sdtout.  
    After option arguments no more arguments should appear.  

    `-c`  
    Code passed data from stdin and send it to sdtout.  
    After option arguments no more arguments should appear.  

    `-p`  
    Create from passed argumnets http post body. It only  
    codes urlcode `data( 2n + 1 )`. So `data1` gets coded  
    but `data2` does not get coded, than ``data3` is coded  
    and `data4` is not coded. And so on..
    
    Dependencies: none  
    Example of using it:  
  >`printf "%s" 'This will get url-encoded' | urlcode -c`  
  >` `  
  >`printf "%s"  'Decode this  url-encoded string' | urlcode -d`  
  >` `  
  >`postbody=$( urlcode '-p' 'login' '=' 'someone' '&' 'pass' '=' 'secret' \`  
  >`                    '&' 'hidden_token' '=' '123jhfierhgiuerg12' )`

    If we plan on further using the postbody it could look like this:  
  >`{ cat httpreq | httpreq_addcrlf; printf "%s" "$postbody"; }  \ `  
  >`|  nc -c -X 5 -x localhost:9050 www 443`  

    As we can see we pass our http request via tor at the final example.  

20. **miodpitny**
  >

    Program will ask you how meany liters of mead ( miodpitny ) you want to make.  
    After that proportions for most common types - czworniak, trojniak, dwojniak and  
    poltorak shall be listed. Telling you how many liters of honey and water you need.  
    How many grams of fosfamon (nutrient medium for yeast ) is needed  
    and how many citrons ( quantity ) should be used for obtaining proper  
    acidity with their juice. Finally how long should the mead be kept in ballon  
    before bottling it. At the end it prints few basic hints.  
    Dependencies: none  
    Example of using it:  
  >`miodpitny`  

23. **irclog** - program to log irc. It's old code. Not too proud of it but I needed to log
             some stuff recently and it worked. So I am adding it here for now.
             Will try to reread it in future for detecting some bugs and other crap.
             How to use: create a file in directory where you will start the irclog program
             The only accepted file name is: irclog.conf
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
28. **fixedxargs** - Program closly performs same task as xargs, BUT PROPERLY.
                Each new line separates new argument. Empty lines creates ''
                Which normal xarg fails to catch. No options. Example:
                echo  "a\nb\nc\nd\n" | fixedxargs echo
29. **listwords** - 
30. **fext**
31. **readNMEA**
32. **findreserved**
33. **version**
34. **getfileinode**
