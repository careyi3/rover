# Rover

To run as a service copy `rover.service` to the location `/lib/systemd/system/`. From here then run the following two commands.

```bash
sudo systemctl enable rover
sudo systemctl start rover
```

Now, once the rover boots, the `start` script will be executed and the rovers firmware will be booted and run in the background.
