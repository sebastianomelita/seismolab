<?php 

/*
Modifacato da Volodymyr Barabash
barabash97@gmail.com
Data: 17-05-2016 12:02
*/
class Socket
{
    /**
     * @var resource
     */
    protected $resource;

    /**
     * @var string
     */
    protected $host;

    /**
     * @var int
     */ 
    protected $port;

    /**
     * @var int
     */
    protected $timeout;

    /**
     * Build a new instance of a socket
     *
     * @param string $host    The ntp server url
     * @param int    $port    The port the ntp server is listening on
     * @param int    $timeout The timeout duration of the connection
     */
    public function __construct($host, $port = 123, $timeout = 5)
    {
        $this->host = $this->resolveAddress($host);
        $this->port = $port;
        $this->timeout = $timeout;

        $this->connect();
    }

    /**
     * Write data to the socket
     *
     * @param string $data The data to write
     *
     * @return void
     */
    public function write($data)
    {
        fwrite($this->resource, $data);
    }

    /**
     * Read data from the socket
     *
     * @throws Exception When the connection timed out
     * @return string
     */
    public function read()
    {
        $info = $this->getMetadata();

        if (true === $info['timed_out']) {
            throw new \Exception('Connection timed out');
        }

        return fread($this->resource, 48);
    }

    /**
     * Closes the socket connection
     *
     * @return void
     */
    public function close()
    {
        fclose($this->resource);
        $this->resource = null;
    }

    /**
     * Check if the connection is open
     *
     * @return bool
     */
    public function isConnected()
    {
        return is_resource($this->resource) && !feof($this->resource);
    }

    /**
     * Gets the full address from the socket
     *
     * @return string|null The address if there is a socket
     */
    public function getAddress()
    {
        if (false !== $this->resource) {
            return stream_socket_get_name($this->resource, false);
        }

        return null;
    }

    /**
     * Gets the ip address from the domain name
     *
     * @param string $host The domain name to resolve
     *
     * @return string
     */
    protected function resolveAddress($host)
    {
        if (filter_var($host, FILTER_VALIDATE_IP)) {
            return $host;
        }

        $ip = gethostbyname($host);
        return "udp://{$ip}";
    }

    /**
     * Returns a stream's meta data
     *
     * @return array
     */
    protected function getMetadata()
    {
        return stream_get_meta_data($this->resource);
    }

    private function connect()
    {
        if (!$this->isConnected()) {
            $this->resource = @fsockopen(
                $this->host, 
                $this->port, 
                $errno, 
                $errstr,
                $this->timeout
            );

            if (!$this->resource) {
                throw new \Exception("Unable to create socket: '{$errno}' '{$errstr}'");
            }
        }
    }
}


class Client
{
    /**
     * @var Socket
     */
    protected $socket;
	
	protected $timestamp_server = 0;
    /**
     * Build a new instance of the ntp client
     *
     * @param Socket $socket The socket used for the connection
     */
    public function __construct(Socket $socket)
    {
        $this->socket = $socket;
    }

    /**
     * Sends a request to the remote ntp server for the current time.
     * The current time returned is UTC.
     *
     * @return \DateTime
     */
    public function getTime()
    {
        $packet = $this->buildPacket();
        $this->write($packet);

        $time = $this->unpack($this->read());
        $time -= 2208988800;

        $this->socket->close();
		
		$this->timestamp = $time + 7200;

        return \DateTime::createFromFormat('U', $time, new \DateTimeZone('UTC'));
    }

    /**
     * Write a request packet to the remote ntp server
     *
     * @param string $packet The packet to send
     *
     * @return void
     */
    protected function write($packet)
    {
        $this->socket->write($packet); 
    }

    /**
     * Reads data returned from the ntp server
     *
     * @return void
     */
    protected function read()
    {
        return $this->socket->read();
    }

    /**
     * Builds the request packet for the current time
     *
     * @return string
     */
    protected function buildPacket()
    {
        $packet = chr(0x1B);
        $packet .= str_repeat(chr(0x00), 47);

        return $packet;
    }

    /**
     * Unpacks the binary data that was returned
     * from the remote ntp server
     * 
     * @param string $binary The binary from the response
     *
     * @return string
     */
    protected function unpack($binary)
    {
        $data = unpack('N12', $binary);
        return sprintf('%u', $data[9]);

    }
}

$socket = new Socket('0.it.pool.ntp.org', 123); 
$ntp = new Client($socket);
$time = $ntp->getTime();

echo "UTC:".$ntp->timestamp."\n";