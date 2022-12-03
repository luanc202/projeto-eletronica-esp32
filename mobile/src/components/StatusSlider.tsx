import { Center, Spinner } from "native-base";

export function StatusSlider() {
  return (
    <Center flex={1} bg="gray.900">
      <Spinner color="yellow.500" />
    </Center >
  );
}