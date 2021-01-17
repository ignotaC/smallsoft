# Small Soft pack
Made for God Glory.
Hail Supreame God one in great trinity.
Prise Saint Maria, mother of God.

A pack of my little unix programs.

1. coin - Program lets you find an answer
          to a 0/1 problem.
2. getproxies - Program filters few webpages
                to suck up proxies, also checks
                if they work properly. Data is
                saved to file "proxylist" in work dir.
                Program uses wget, so it is needed
                to work.
3. novena - It will save file in home directory
            and track with it day of novena.
            All you need to do is enter the name
            of current novena, also you can
            set time, if you started two days
            ago the prayer - still will work fine.
            Creates .novena file in env HOME
4. workout - Program generates workout entries
             for each day. Keep training!
             Creates .workout file in env HOME
5. logtime - Logs time from stdin per line and
             attaches it on stdout.
6. recev - Record events from mouse and keyboard.
           It needs to work xinput, and xmodmap.
           Also xev might be useful. Record before
           processing (all happen in one run) 
           might make a big file. You tell the
           program key you choose to start/stop
           recording and file where he saves
           the record. To play recorded events
           use playev program from this list.
7. playev - Program playes events saved in file
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
8. wh_frktcp - Program sets up chosen ammoun of
               processess that will run an internet
               command using tcp. It will use torsocks
               all times. So we keep privacy.
               This mean you need to install torsocks.
               Program will guide you how to use it.
9. wh_inchttpext - Program will go through specified
                   link incrementing http link each time,
                   in position you point like http://xxx.xx/%lld.
                   In the %lld will be put number. just run program
                   without arguments and it will guid you how to
                   use it. The program was made for extracting data
                   from websites. I find it sometimes useful.
                   It will try to find phrase in downloaded web
                   page source. 
10. wh_httpext - In general it's a string extractor form a file.
                 It has three basic commands. Pass to pass file
                 untill a string is met. Cut that find strings
                 in line and outputs on stdout string between them.
                 last command is loop. It will loop for ever last
                 command. Only has sens to use after cut command.
                 Ofc loop will end with eof at stdin. If you 
                 don't pass any command *help prompt* will appear. 
11. linsaftrdiff - You pass two file names. Program prints on stdout
                   all lines of first passed file after first different
                   line is met in other file.
12. fstrswp - Swap one string in stream to other, only files with lines
              shorter than 8192 for now. Perfect for some source fixing.
              For example when we need to swap malloc.h to stdlib.h.
              this would look like this:
              find src -type f -iname '*.c' -exec sh -c \
                   'cat {} | fstrswp '\''malloc.h'\'' '\''stdlib.h'\'' ' \
                   '| tmpfile; mv tmpfile {}' \;
              Could be without '\'' but what if we have spaces or worse?
13. parsekdump - This is wrote for openbsd ktrace output. Not sure how it
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
14. ualist - Program will output little list of user agents to stdout.
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
15. randstr - Program generates using pseudorandom generator ( C rand() )
              Randomn string. You must pass number of bytes to generate.
              Also you can force to output onlu uppercase letters, lowercase
              and digits. With commands -u -l -d.
              Example: randstr -u 124 -ld. This will generate 124 bytes
              that in asci code appear as lowercase uppercase or digits.
16. logdata - Program opens unix socket and prints all data sent to it.
              Example of using it:
              We start it: logdata unix_socket_name_we_pass > savehere
              Now we need to send some data using netcat.
              Openbsd netcat can send to unix socket. 
              Many linux distros have in their repository: openbsd-netcat
              or something similar. So we can with nc:
              echo "test" | nc -UN socket_name
              -N option is a must. Socket must sent FIN else the connection stales.
              SIGINT logdata for gentle close when not anymore needed!
17. runprog - Program opens unix socket and runs with system C syscall
              command we pass at it start up. It runs the command when
              there is opened connection. It ignores sent data.  You can either
              not even wait for the program to finish running the command
              or wait.
              Example: ./runprog '/tmp/sun' './runthisscript';
              Than ask it to run it but don't wait for it to finish: nc -Uz '/tmp/sun'
              Or ask it to run it and wait till it finish: nc -Ud '/tmp/sun'
              Runprog closes gently when recives sigint. Additional one more argument
              is for groups. You can do:
              ./runprog '/tmp/sun' './runthisscript' 'go'
              last one go stand for group and other - write permissions set on
              unix socket. So if you need root to execute something simple user
              can call it via this socket to do the job.
18. httpreq_addcrlf - Program adds after new line of each entry CRLF creating
              proper http request. On the end it makes passes one more CRLF.
              Empty lines are being ignored. Example of usage, we have file httpreq:
              
              GET /babylon/tower HTTP/1.0
              Host: gagle.shit
              
              We use it this way:
              cat httpreq | httpreq_addcrlf | netcat gagle.shit 80
19. flinemem - program loads stdin line to memory and searches if passed arguments
	      exist in line. Line is printed to stdout only if all strings exist in line.
              No need for regular expression when you simply need to find static patterns.
              Example fo using:  cat file | flinemem 'pattern1' 'pattern2' 'pattern3' 

20. urlcode - at the moment we have 3 options, encode (-e), decode (-d)  and
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
              { cat httpreq | httpreq_addcrlf | cat; printf "%s" "$postbody" } | \
              nc -c -X 5 -x localhost:9050 www:443
 
              Hope this is clear. The last line shows nc with passing traffic via tor
              for server with https.

How to install?
Run make in makefile directory.
Compiled programs will go to the bin directory
inside the directory of your makefile.
In the end you just copy what ever you like
to your env PATH
