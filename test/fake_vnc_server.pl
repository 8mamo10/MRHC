use strict;
use warnings;
use Socket;

my $sock_receive;
socket($sock_receive, PF_INET, SOCK_STREAM, getprotobyname('tcp'))
    or die "Cannot create socket: $!";

my $local_port = 6624;

my $pack_addr = sockaddr_in($local_port, INADDR_ANY);

bind($sock_receive, $pack_addr)
    or die "Cannot bind: $!";

listen($sock_receive, SOMAXCONN)
    or die "Cannot listen: $!";

my $sock_client;

while (accept($sock_client, $sock_receive)) {
    my $version = pack("CCCCCCCCCCCC", 0x52, 0x46, 0x42, 0x20, 0x30, 0x30, 0x33, 0x2e, 0x30, 0x30, 0x38, 0x0a);
    print $sock_client "$version";
    $sock_client->autoflush;
    my $content = "";

    while (my $line = <$sock_client>) {
        $content .= $line;
    }
    print $content;
    STDOUT->autoflush;
    close $sock_client;
}
